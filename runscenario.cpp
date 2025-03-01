#include "scenariogen.cpp"


int main() {
    constexpr float risk_free_rate = 1.2/100;
    constexpr float dt = 1.0/252;
    
    Macro USA(risk_free_rate, dt);
    Equity AAPL(165, 0.05, USA);

    EquityPriceGenerator epg(AAPL, USA);
    constexpr int num_steps = 50;
    constexpr int num_scenarios = 100000;

    vector<vector<double>> scenarios = epg.generateScenarios(num_steps, num_scenarios);

    vector<double> scenario_0 = scenarios[0];

    for (const double price : scenario_0) {
        cout << price << "\n";
    }
    string filename = "scenarios.csv";
    save_as_csv(scenarios, filename);

    
    return 0;
}