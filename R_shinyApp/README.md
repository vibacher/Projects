# Interactive Data Exploration Dashboard (R Shiny)

An interactive web application built in R Shiny for exploring, filtering, and visualising structured JSON datasets (`data_cia2.json`). The dataset had to be tidied, analysed and visualised.

## Files
- `app.R` – Complete Shiny application containing both the reactive UI layout and server-side data processing/plotting logic.
- `data_cia2.json` – dataset by **CIA World Factbook** (2020 country profiles, 227 countries and territories) 
enriched with World Bank income classifications (`status`) and demographic indicators (`youth_unempl_rate`, `net_migr_rate`, `electricity_fossil_fuel`, `life_expectancy`).

## How to Run
```r
install.packages(c("shiny", "jsonlite", "ggplot2", "dplyr", "DT", "plotly", "countrycode"))
shiny::runApp("app.R")
