#include "scenariogen.cpp"


int main() {
    constexpr float risk_free_rate = 1.2/100;
    constexpr float dt = 1.0/252;
    
    Macro USA(risk_free_rate, dt, 80);
    Equity AAPL(165, 0.05, USA);

    EquityPriceGenerator epg(AAPL, USA);
    constexpr int num_steps = 500;
    constexpr int num_scenarios = 1e5;

    ScenarioMatrix scenarios = epg.generateScenarios(num_steps, num_scenarios);

    string filename = "scenarios.csv";
    save_as_csv(scenarios, filename);


    return 0;
}