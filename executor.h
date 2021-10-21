#ifndef EXECUTOR_GUARD_H
#define EXECUTOR_GUARD_H

#include <map>
#include <omp.h>
#include <vector>

namespace nuworks {
    typedef std::map<std::pair<int, int>, double> LUT;

    struct Way {
        std::vector<int> nodes;
        double fitness = 0.0;
    };

    struct Node {
        int id, x, y;
        double distanceTo(const Node &node) const;
    };

    class Executor {
    private:
        Way _best_population{};

        int citiesCount;
        int populationCount;
        double mutationChance = 0.2;

        std::vector<Node> _cities;
        std::vector<Way> _population;
        LUT _dist_lut;

    public:
        Executor(int citiesCount, int populationCount) {
            this->citiesCount = citiesCount;
            this->populationCount = populationCount;

            _cities.reserve(citiesCount);
            _population.reserve(populationCount);
        }

        void generate_cities();
        void generate_lut();
        void generate_first_population();
        void load_map();
        void write_map();
        void init();
        double get_way_length(const Way &way);
        void calc_fitness();
        std::pair<int, int> find_two_best();
        void next_generation();
        std::pair<Way, Way> cross_over(const Way &p1, const Way &p2);
        void mutate(Way &w);

        void print_lut();
        void print_stats();

        int getCitiesCount();
        int getPopulationCount();
    };
} // namespace nuworks

#endif // EXECUTOR_GUARD_H
