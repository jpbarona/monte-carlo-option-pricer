#include <iostream>
#include <random>
#include <vector>
#include <fstream>
using std::mt19937;
using std::normal_distribution;
using namespace std;
#define DEFAULT_SEED 69
#define DEFAULT_DT 252.0
#define DEFAULT_RISK_FREE_RATE 1.2/100

//Holds simulation data, metadata
struct ScenarioMatrix{
    vector<double> scenarios;
    const int num_steps;
    const int num_scenarios;
    ScenarioMatrix(int num_steps, int num_scenarios) : num_steps(num_steps), num_scenarios(num_scenarios) {
        scenarios.resize(num_scenarios*num_steps);
    }
};

/*These macro objects act like configuration files for simulation parameters
and macroeconomic factors like the risk free rate. Each equity must have a Macro
to initialize.*/

struct Macro {
    double riskfreerate;
    double dt;
    int seed;
    Macro (double r=DEFAULT_RISK_FREE_RATE, double dt=DEFAULT_DT, int seed=DEFAULT_SEED) : riskfreerate(r), dt(dt), seed(seed) {}
};

//If no macro is given, assumes default values set in definitions.
struct Equity : public Macro {
    double price;
    double volatility;

    Equity(double p, double v, const Macro &m = Macro()) : Macro(m), price(p), volatility(v) {}
};

void save_as_csv(ScenarioMatrix scenario_matrix, string filename) {
    ofstream file(filename);
    int num_scenarios = scenario_matrix.num_scenarios;
    int num_steps = scenario_matrix.num_steps;
    vector<double>& scenarios = scenario_matrix.scenarios;

    for (int scenario = 0; scenario < num_scenarios; scenario++)
    {
        for (int time = 0; time < num_steps; time++)
        {
            file << scenarios[scenario*num_steps + time];
            if (time != num_steps-1) {
                file << ",";
            }
        }
        file << "\n";        
    }
    file.close();
    cout << "Prices saved to " + filename << endl;
    
}


class EquityPriceGenerator {
    public:
    double rRate;
    double volatility;
    double dt;
    double initial_price;
    mt19937_64 mtEngine;
    normal_distribution<> nd;

    EquityPriceGenerator(Equity e, Macro m) : rRate(m.riskfreerate), volatility(e.volatility), dt(m.dt), initial_price(e.price), mtEngine(m.seed) {}
    

    ScenarioMatrix generateScenarios(int num_steps, int num_scenarios) {
        ScenarioMatrix scenario_matrix = ScenarioMatrix(num_steps, num_scenarios);
        set_initial_price(scenario_matrix, this->initial_price);
        for (int time = 1; time < num_steps; time++)
        {
            generate_scenario_vector(scenario_matrix, time);
        }
        return scenario_matrix;
    }
    
    private:
    //Wiener Process (no drift)
    const double expArg1 = (rRate - (volatility * volatility / 2.0)) * dt;
    double nextPrice(double current_price, double norm) {
        double expArg2 = volatility * sqrt(dt) * norm;
        double newprice = current_price * exp(expArg1 + expArg2);
        return newprice;
    }

    void generate_scenario_vector(ScenarioMatrix &scenario_matrix, int time) {
        vector<double>& scenarios = scenario_matrix.scenarios;
        
        int num_scenarios = scenario_matrix.num_scenarios;
        int num_steps = scenario_matrix.num_steps;
        for (int scenario = 0; scenario < num_scenarios; scenario++)
        {
            double norm = nd(mtEngine);
            double current_price = scenarios[scenario*num_steps + time-1];
            double next_price = nextPrice(current_price, norm);
            scenarios[scenario*num_steps + time] = next_price;
        }
    }
    void set_initial_price(ScenarioMatrix &scenario_matrix, double price) {
        int num_scenarios = scenario_matrix.num_scenarios;
        int num_steps = scenario_matrix.num_steps;
        vector<double>& scenarios = scenario_matrix.scenarios;
        for (int scenario = 0; scenario < num_scenarios; scenario++)
        {
            scenarios[scenario*num_steps] = price;
        }   
    }

};