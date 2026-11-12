library(shiny)
library(ggplot2)
library(dplyr)
library(jsonlite)
library(DT)
library(plotly)
library(countrycode)

world_map <- map_data("world")
world_map$ISO3 <- countrycode::countrycode(
  sourcevar = world_map$region,
  origin = "country.name",
  destination = "iso3c", nomatch = NA
)



cia_data <- jsonlite::fromJSON("./data/data_cia2.json")
world_map <- dplyr::left_join(world_map, cia_data, by = "ISO3")

map_vars <- c(
  "Expenditure on education" = "expenditure",
  "Youth unemployment rate" = "youth_unempl_rate",
  "Net migration rate" = "net_migr_rate",
  "Electricity from fossil fuels" = "electricity_fossil_fuel",
  "Area" = "area",
  "Population growth rate" = "pop_growth_rate",
  "Life expectancy at birth" = "life_expectancy",
  "Population" = "population"
)

ui <- fluidPage(
  titlePanel("CIA World Factbook 2020"),
  tags$p("Welcome to my shiny app, which allows you to visualize variables from the CIA 2020 factbook on the world map, generate descriptive statistics and statistical graphics."),
  tabsetPanel(
    tabPanel(
      "Univariate analysis",
      sidebarLayout(
        sidebarPanel(
          h4("Select a variable:"),
          selectInput("variable",
            label = NULL,
            choices = map_vars,
            selected = "expenditure"
          ),
          actionButton("view_data", "View raw data"),
          hr(),
          DTOutput("raw_table")
        ),
        mainPanel(
          tabsetPanel(
            tabPanel(
              "Map",
              tags$p("The map contains values of the selected variables. The countries with gray areas have a missing value for the visualized variable."),
              plotlyOutput("map_plot", height = "500px")
            ),
            tabPanel(
              "Global analysis",
              plotlyOutput("global_plot", height = "500px")
            ),
            tabPanel(
              "Analysis per continent",
              plotlyOutput("continent_plot", height = "500px")
            )
          )
        )
      )
    ),
    tabPanel(
      "Multivariate analysis",
      fluidRow(
        sidebarLayout(
          sidebarPanel(
            selectInput("xaxis", "Select variable 1: ", choices = map_vars, selected = "expenditure"),
            selectInput("yaxis", "Select variable 2: ", choices = map_vars, selected = "youth_unempl_rate"),
            selectInput("sizing", "Scale by: ", choices = c("Population" = "population", "Area" = "area"), selected = "area")
          ),
          mainPanel(
            h4("Scatterplot"),
            plotlyOutput("scatter_plot", height = "500px")
          )
        )
      )
    )
  )
)


server <- function(input, output) {
  selected_var <- reactive(input$variable)
  selected_label <- reactive({
    names(map_vars)[map_vars == selected_var()][1]
  })

  raw_table_data <- eventReactive(input$view_data, {
    table_data <- world_map %>%
      transmute(
        country = dplyr::coalesce(country, region),
        continent = continent,
        value = .data[[selected_var()]]
      ) %>%
      distinct()

    names(table_data)[3] <- selected_label()
    table_data
  })

  output$raw_table <- DT::renderDT({
    req(raw_table_data())
    DT::datatable(
      raw_table_data(),
      rownames = FALSE,
      class = "compact",
      options = list(
        pageLength = 15,
        lengthChange = TRUE,
        autoWidth = TRUE
      )
    )
  })

  output$map_plot <- plotly::renderPlotly({
    map_data <- world_map %>%
      mutate(
        country_label = dplyr::coalesce(country, region),
        value_label = .data[[selected_var()]],
        tooltip = paste0(country_label, ": ", value_label)
      )

    map_plot <- ggplot(map_data, aes(x = long, y = lat, group = group)) +
      geom_polygon(
        aes(fill = .data[[selected_var()]], text = tooltip),
        colour = "white",
        linewidth = 0.2
      ) +
      scale_fill_viridis_c(na.value = "grey80") +
      labs(fill = selected_label())

    plotly::ggplotly(map_plot, tooltip = "text") %>%
      plotly::config(displayModeBar = FALSE)
  })

  output$global_plot <- plotly::renderPlotly({
    plot_data <- world_map %>%
      distinct(country, continent, .data[[selected_var()]]) %>%
      filter(!is.na(.data[[selected_var()]]))

    hist_plot <- ggplot(plot_data, aes(x = .data[[selected_var()]])) +
      geom_histogram(aes(y = after_stat(density)), bins = 25, fill = "#9aa6b2", alpha = 0.6) +
      geom_density(fill = "#6c7aa1", alpha = 0.35) +
      labs(x = selected_label(), y = "density")
    
    box_plot <- ggplot(plot_data, aes(y = .data[[selected_var()]])) +
      geom_boxplot(fill = "#ffffff") +
      labs(y = selected_label(), x = NULL)

    plotly::subplot(
      plotly::ggplotly(hist_plot, tooltip = c("x", "y")),
      plotly::ggplotly(box_plot, tooltip = "y"),
      nrows = 1,
      shareY = FALSE,
      margin = 0.06
    ) %>%
      plotly::layout(
        xaxis = list(title = selected_label()),
        yaxis = list(title = "Density"),
        xaxis2 = list(title = ""),
        yaxis2 = list(title = selected_label())
      ) %>%
      plotly::config(displayModeBar = FALSE)
  })

  output$continent_plot <- plotly::renderPlotly({
    plot_data <- world_map %>%
      distinct(country, continent, .data[[selected_var()]]) %>%
      filter(!is.na(continent), !is.na(.data[[selected_var()]]))

    box_plot <- ggplot(plot_data, aes(x = continent, y = .data[[selected_var()]], fill = continent)) +
      geom_boxplot(outlier.alpha = 0.6) +
      labs(x = NULL, y = selected_label()) +
      guides(fill = "none")

    density_plot <- ggplot(plot_data, aes(x = .data[[selected_var()]], color = continent, fill = continent)) +
      geom_density(alpha = 0.2) +
      labs(x = selected_label(), y = "density")

    plotly::subplot(
      plotly::ggplotly(density_plot, tooltip = c("x", "y", "colour")),
      plotly::ggplotly(box_plot, tooltip = c("x", "y")),
      nrows = 1,
      shareY = FALSE,
      margin = 0.06
    ) %>%
      plotly::layout(
        xaxis = list(title = selected_label()),
        yaxis = list(title = "Density"),
        xaxis2 = list(title = ""),
        yaxis2 = list(title = selected_label())
      ) %>%
      plotly::config(displayModeBar = FALSE)
  })
  
  x_var <- reactive(input$xaxis)
  y_var <- reactive(input$yaxis)
  size_var <- reactive(input$sizing)
  
  
  output$scatter_plot <- renderPlotly({
    plot_data <- world_map %>% distinct(
      country, continent, .data[[x_var()]],
      .data[[y_var()]], .data[[size_var()]]
      ) %>%
      filter(!is.na(continent),!is.na(.data[[x_var()]]),
      !is.na(.data[[y_var()]]), !is.na(.data[[size_var()]])
      )
    
    p <- ggplot(plot_data, aes(x= .data[[x_var()]], y = .data[[y_var()]], color = continent)) +
      geom_point(aes(size = .data[[size_var()]])) +
      geom_smooth(method = "loess", se = FALSE, aes(group = continent)) +
      labs(x = names(map_vars)[map_vars == x_var()],
           y = names(map_vars)[map_vars == y_var()],
           size = names(map_vars)[map_vars == size_var()]) +
      theme_minimal()
    
    ggplotly(p)
  })
}

shinyApp(ui = ui, server = server)
