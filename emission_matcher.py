import time # to compare the methods; important for large datasets
from pathlib import Path # for independent file paths
import numpy as np
import pandas as pd
from rapidfuzz import fuzz, process, utils                        
from sentence_transformers import SentenceTransformer       
from sklearn.feature_extraction.text import TfidfVectorizer 
from sklearn.metrics.pairwise import cosine_similarity      

UNKNOWN_ID = "UNKNOWN" # prediction if a method has no similarity at all
N_RUNS = 5             # runs per method, we report the median time


def run_fuzzy_matching(queries, categories, catalogue_ids): # with rapidfuzz
    '''
    :param queries: list of product names
    :param categories: list of category texts like "Dairy products: milk, cheese"
    :param catalogue_ids: list of category ids, EF01 - EF012
    
    algorithm writes queries and categories in lower case and splits them into tokens
    compares the intersection and the set difference
    calculates distance based ratio --> assigns value based on how similar they are
    Pro: very fast and robust against typos
    Contra: translations like Gipfeli = Croissants don't work
    '''

    preds = []
    scores = []

    for query in queries: # compares query with all categories and gives back the index of the most similar
        _, score, idx = process.extractOne(query, categories, scorer=fuzz.token_set_ratio, processor=utils.default_process)
        preds.append(catalogue_ids[idx])   
        scores.append(round(score / 100, 3))

    return preds, scores


def run_tfidf_matching(queries, categories, catalogue_ids): # with scikit-learn
    '''
    :param queries: list of product names
    :param categories: list of category texts like "Dairy products: milk, cheese"
    :param catalogue_ids: list of category ids, EF01 - EF012

    algorithm builds a matrix based on the vocabulary it learned and projects it into its vector space 
    Then it takes the cosine: 1 = identical, 0 = no similarity

    Pro:  higher accuracy than fuzzy matching
    Contra: unknown words are assigned to the null vector and thus land on the first category id --> mark them as UNKNOWN insteas
    '''
    vec = TfidfVectorizer(ngram_range=(1, 2))

    cat_matrix = vec.fit_transform(categories) # builds a matrix based on the learned vocabulary 
    query_matrix = vec.transform(queries) # projects queries in the same vector space
    sim = cosine_similarity(query_matrix, cat_matrix) # calculates angle between query_matrix and cat_matrix

    best_idx = np.argmax(sim, axis=1)
    best_score = np.max(sim, axis=1)
    preds = [catalogue_ids[i] if s > 0 else UNKNOWN_ID for i, s in zip(best_idx, best_score)]
    scores = [round(float(s), 3) for s in best_score]

    return preds, scores


def run_embedding_matching(queries, categories, catalogue_ids, model):
    '''
    :param queries: list of product names
    :param categories: list of category texts like "Dairy products: milk, cheese"
    :param catalogue_ids: list of category ids, EF01 - EF012
    :param model: loaded SentenceTransformer (loaded once in main, not part of the timing)

    algorithm based on deep learning. it puts sentences in a 384-dim vector space. 
    The more similar the closer they are together.

    Pro: Chosen model is fast and knows over 50 languages (e.g. German and English)
    Contra: dominant words can lead to wrong results: e.g. oat milk --> milk --> cows milk
    '''
    cat_emb = model.encode(categories, normalize_embeddings=True) # puts the categories in a 384 dimensional unit vector
    query_emb = model.encode(queries, normalize_embeddings=True) # puts query in a 384 dimensional unit vector

    sim = query_emb @ cat_emb.T # cosine similarity is matrix multiplication as we are dealing with unit vectors
    best_idx = np.argmax(sim, axis=1)
    preds = [catalogue_ids[i] for i in best_idx]
    scores = [round(float(s), 3) for s in np.max(sim, axis=1)]

    return preds, scores

# after a look into the results: combine tfidf and embedding
def run_hybrid_matching(queries, categories, catalogue_ids, model):
    t_preds, t_scores = run_tfidf_matching(queries, categories, catalogue_ids)
    e_preds, e_scores = run_embedding_matching(queries, categories, catalogue_ids, model)
    preds, scores = [], []
    for tp, ts, ep, es in zip(t_preds, t_scores, e_preds, e_scores):
        if tp == UNKNOWN_ID: # TF-IDF found nothing --> trust the embeddings
            preds.append(ep); scores.append(es)
        else:
            preds.append(tp); scores.append(ts)
    return preds, scores


def benchmark(func, *args, n_runs=N_RUNS):
    '''
    runs a method several times and returns the result plus the median time in ms
    '''
    times = []
    for _ in range(n_runs):
        t0 = time.perf_counter()
        preds, scores = func(*args)
        times.append((time.perf_counter() - t0) * 1000)
    return preds, scores, float(np.median(times))


def score_predictions(name, preds, scores, runtime_ms, items_df, ef_map, true_co2e):
    accuracy = (np.array(preds) == items_df["ground_truth_id"].to_numpy()).mean()
    fallback_ef = float(np.mean(list(ef_map.values())))
    est_co2e = sum(spend * ef_map.get(pred, fallback_ef) for spend, pred in zip(items_df["spend_chf"], preds))
    error_pct = ((est_co2e - true_co2e) / true_co2e) * 100

    return {
        "name": name,
        "accuracy": accuracy,
        "runtime_ms": runtime_ms,
        "est_co2e": est_co2e,
        "error_pct": error_pct,
        "preds": preds,
        "scores": scores,
    }


def main():
    data_dir = Path(__file__).resolve().parent / "data"
    items_df = pd.read_csv(data_dir / "items.csv")
    ef_df = pd.read_csv(data_dir / "emission.csv")

    # Combine category title and examples for richer text matching
    categories = (ef_df["category_name"] + ": " + ef_df["description"]).tolist()
    catalogue_ids = ef_df["category_id"].tolist()
    ef_map = dict(zip(ef_df["category_id"], ef_df["ef_kg_co2e_per_chf"])) #dictionary for emission factors

    # fail if the ground truth contains an id without emission factor
    missing = set(items_df["ground_truth_id"]) - set(ef_map)
    if missing:
        raise ValueError(f"ground_truth_id not in emission.csv: {sorted(missing)}")

    true_co2e = sum(spend * ef_map[gt] for spend, gt in zip(items_df["spend_chf"], items_df["ground_truth_id"]))
    queries = items_df["raw_description"].tolist()

    # Load model once as first call takes longer
    model = SentenceTransformer("paraphrase-multilingual-MiniLM-L12-v2")
    model.encode(["warm-up"])

    methods = [
        ("1. Fuzzy String (RapidFuzz)", benchmark(run_fuzzy_matching,queries, categories, catalogue_ids)),
        ("2. TF-IDF + Cosine Sim", benchmark(run_tfidf_matching, queries, categories, catalogue_ids)),
        ("3. Multilingual Embeddings", benchmark(run_embedding_matching, queries, categories, catalogue_ids, model)),
        ("4. Hybrid Matching", benchmark(run_hybrid_matching,queries, categories, catalogue_ids, model)),
    ]

    results = [
        score_predictions(name, preds, scores, rt, items_df, ef_map, true_co2e)
        for name, (preds, scores, rt) in methods
    ]

    # Print summary table
    print(f"\n### Comparison of Methods (Ground Truth Total: {true_co2e:.2f} kg CO2e)\n")
    summary_df = pd.DataFrame(
        [
            {
                "Method": r["name"],
                "Accuracy": f"{r['accuracy']:.1%}",
                "Est. Total (kg CO2e)": f"{r['est_co2e']:.2f}",
                "GHG Error (%)": f"{r['error_pct']:+.1f}%",
                "Median Time": f"{r['runtime_ms']:.1f} ms",
            }
            for r in results
        ]
    )
    print(summary_df.to_markdown(index=False))

    print("\n### Item-Level Predictions\n")
    details_df = items_df[["item_id", "raw_description", "ground_truth_id"]].copy()

    for label, res in zip(["Fuzzy", "TF-IDF", "Embeddings", "Hybrid"], results):
        preds_col = pd.Series(res["preds"])
        scores_col = pd.Series(res["scores"]).map("{:.2f}".format)
        details_df[f"{label}_Pred"] = preds_col + " (" + scores_col + ")"

    # Save to Markdown file
    output_path = Path(__file__).resolve().parent / "benchmark_results.md"
    report = (
        f"### Comparison of Methods (Ground Truth Total: {true_co2e:.2f} kg CO2e)\n\n"
        f"{summary_df.to_markdown(index=False)}\n\n"
        f"### Item-Level Predictions\n\n"
        f"{details_df.to_markdown(index=False)}\n"
    )
    output_path.write_text(report, encoding="utf-8")
    print(f"Results saved to {output_path}")

if __name__ == "__main__":
    main()