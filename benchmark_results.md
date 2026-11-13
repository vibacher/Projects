### Comparison of Methods (Ground Truth Total: 5230.30 kg CO2e)

| Method                      | Accuracy   |   Est. Total (kg CO2e) | GHG Error (%)   | Median Time   |
|:----------------------------|:-----------|-----------------------:|:----------------|:--------------|
| 1. Fuzzy String (RapidFuzz) | 64.3%      |                5221.07 | -0.2%           | 0.2 ms        |
| 2. TF-IDF + Cosine Sim      | 57.1%      |                7232.08 | +38.3%          | 1.0 ms        |
| 3. Multilingual Embeddings  | 64.3%      |                6449.62 | +23.3%          | 64.4 ms       |
| 4. Hybrid Matching          | 78.6%      |                5280.43 | +1.0%           | 59.5 ms       |

### Item-Level Predictions

| item_id   | raw_description                               | ground_truth_id   | Fuzzy_Pred   | TF-IDF_Pred    | Embeddings_Pred   | Hybrid_Pred   |
|:----------|:----------------------------------------------|:------------------|:-------------|:---------------|:------------------|:--------------|
| P01       | Oat milk Barista 1L tetra pak (case of 12)    | EF02              | EF02 (0.47)  | EF02 (0.45)    | EF01 (0.61)       | EF02 (0.45)   |
| P02       | Eppendorf pipette tips 200ul sterile racks    | EF07              | EF07 (0.44)  | EF07 (0.35)    | EF07 (0.43)       | EF07 (0.35)   |
| P03       | Lenovo ThinkPad X1 Carbon Gen 11 workstation  | EF09              | EF09 (0.43)  | UNKNOWN (0.00) | EF09 (0.39)       | EF09 (0.39)   |
| P04       | Bio Vollmilch 3.5% für Mensa Kaffeemaschine   | EF01              | EF05 (0.36)  | UNKNOWN (0.00) | EF06 (0.51)       | EF06 (0.51)   |
| P05       | Vegetarian lunch buffet catering for workshop | EF12              | EF12 (0.64)  | EF12 (0.45)    | EF12 (0.77)       | EF12 (0.45)   |
| P06       | Ethanol 99.8% analytical grade 2.5L bottle    | EF08              | EF08 (0.44)  | EF08 (0.24)    | EF08 (0.65)       | EF08 (0.24)   |
| P07       | Fairtrade Arabica espresso beans 1kg          | EF06              | EF06 (0.56)  | EF06 (0.29)    | EF06 (0.51)       | EF06 (0.29)   |
| P08       | Frische Gipfeli und Sauerteigbrot Frühstück   | EF05              | EF11 (0.37)  | UNKNOWN (0.00) | EF05 (0.50)       | EF05 (0.50)   |
| P09       | Digital fluorescence microscope camera module | EF10              | EF10 (0.40)  | UNKNOWN (0.00) | EF10 (0.59)       | EF10 (0.59)   |
| P10       | Haferdrink ungesüsst 12x1L Karton             | EF02              | EF08 (0.37)  | UNKNOWN (0.00) | EF04 (0.22)       | EF04 (0.22)   |
| P11       | Beef burger patties frozen 5kg box            | EF04              | EF04 (0.40)  | EF04 (0.24)    | EF04 (0.59)       | EF04 (0.24)   |
| P12       | A4 recycled printer paper 500 sheets x5       | EF11              | EF12 (0.39)  | EF11 (0.41)    | EF11 (0.55)       | EF11 (0.41)   |
| P13       | Nitril Laborhandschuhe steril Grösse M        | EF07              | EF06 (0.36)  | UNKNOWN (0.00) | EF10 (0.39)       | EF10 (0.39)   |
| P14       | Apéro Riche vegetarisch für Institutsanlass   | EF12              | EF12 (0.47)  | EF12 (0.20)    | EF04 (0.38)       | EF12 (0.20)   |
