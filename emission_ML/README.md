# Procurement emission factor matcher
Assigning purchase line items (e.g. "Oat milk Barista 1L tetra pak (case of 12)") 
to emission categories, so that spend-based CO2e estimates can be calculated automatically.
The project compares four matching approaches on a small labelled, synthetic example dataset.

## Overview
Procurement data is often multilingual. A spend-based GHG inventory needs every item mapped to a category with an emission factor.
A wrong category lowers accuracy and it also shifts the estimated total.

1. **Fuzzy String Matching (`RapidFuzz`):** Token-based matching and distance scoring.
   Fast and resilient to typos or word reordering, but unable to bridge synonyms of different languages without lexical overlap (e.g. Gipfeli ≠ Croissant).
2. **TF-IDF + Cosine Similarity (`scikit-learn`):** Bag-of-words vectorisation using unigrams and bigrams.
   Captures exact domain keywords well; zero-similarity items (`score == 0`) are explicitly flagged as `UNKNOWN` rather than defaulting blindly to index `0`.
3. **Multilingual Dense Embeddings (`sentence-transformers`):** Semantic similarity in a 384-dimensional unit vector space using `paraphrase-multilingual-MiniLM-L12-v2`,
   supporting cross-lingual matching across 50+ languages (German and English included).
4. **Hybrid Case (`TF-IDF` → `Embeddings`):** Uses `TF-IDF` when lexical keyword overlap exists (avoiding semantic token bias such as *"Oat milk"* → *"Dairy"*),
    and falls back to multilingual dense embeddings whenever `TF-IDF` returns `UNKNOWN`.


## Results
The results can be seen in [benchmark_results.md](benchmark_results.md).

## Limitations
- The synthetic example dataset is very small (14 items). Thus, differences in method scores are not statistically reliable.
- Note, that the Hybrid Case was implemented after having a look at the data.
- Items predicted as `UNKNOWN` are calculated with the average emission factor of their categories.
- Three items (all German-language) are misclassified by every method, likely because the category descriptions are English only.

## Usage

```bash
pip install -r requirements.txt
python emission_matcher.py
```
