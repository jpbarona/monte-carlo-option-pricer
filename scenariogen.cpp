#include <iostream>
#include <random>
#include <vector>
#include <fstream>
using std::mt19937;
using std::normal_distribution;
using namespace std;
#define DEFAULT_SEED 69
#define DT 252.0

struct ScenarioMatrix{
    vector<vector<double>> scenarios;
    int num_steps;
    int num_scenarios;
    ScenarioMatrix(int num_steps, int num_scenarios) : num_steps(num_steps), num_scenarios(num_scenarios) {
        scenarios.resize(num_scenarios+1);
        for (int i = 0; i < num_scenarios; i++)
        {
            scenarios[i].resize(num_steps);
        }
    }
};

struct Macro {
    double riskfreerate;
    double dt;
    int seed;
    Macro (double r, double dt=DT, int seed=DEFAULT_SEED) : riskfreerate(r), dt(dt), seed(seed) {}
};

struct Equity : public Macro {
    double price;
    double volatility;

    Equity(double p, double v, const Macro &m) : Macro(m), price(p), volatility(v) {}
};


class EquityPriceGenerator {
    public:
    double rRate;
    double volatility;
    double dt;
    double initial_price;
    mt19937_64 mtEngine;
    normal_distribution<> nd;

    EquityPriceGenerator(Equity e, Macro m) : rRate(m.riskfreerate), volatility(e.volatility), dt(m.dt), initial_price(e.price), mtEngine(m.seed) {}
    

    vector<vector<double>> generateScenarios(int num_steps, int num_scenarios) {
        ScenarioMatrix scenario_matrix = ScenarioMatrix(num_steps, num_scenarios);
        set_initial_price(scenario_matrix, this->initial_price);
        for (int time = 1; time < num_steps; time++)
        {
            generate_scenario_vector(scenario_matrix, time);
        }
        return scenario_matrix.scenarios;
    }
    
    private:
    const double expArg1 = (rRate - (volatility * volatility / 2.0)) * dt;
    double nextPrice(double current_price, double norm) {

        double expArg2 = volatility * sqrt(dt) * norm;
        double newprice = current_price * exp(expArg1 + expArg2);
        return newprice;
    }

    void generate_scenario_vector(ScenarioMatrix &scenario_matrix, int time) {
        vector<vector<double>>& scenarios = scenario_matrix.scenarios;
        int num_scenarios = scenario_matrix.num_scenarios;
        for (int scenario = 0; scenario < num_scenarios; scenario++)
        {
            double norm = nd(mtEngine);
            double current_price = scenarios[scenario][time-1];
            double next_price = nextPrice(current_price, norm);
            scenarios[scenario][time] = next_price;
        }
    }
    void set_initial_price(ScenarioMatrix &scenario_matrix, double price) {
        int num_scenarios = scenario_matrix.num_scenarios;
        vector<vector<double>>& scenarios = scenario_matrix.scenarios;
        for (int scenario = 0; scenario < num_scenarios; scenario++)
        {
            scenarios[scenario][0] = price;
        }   
    }

};