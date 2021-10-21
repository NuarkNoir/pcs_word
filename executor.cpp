#include "executor.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <float.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

#define SCREEN_WIDTH 100

std::random_device r;
std::default_random_engine el(r());
std::uniform_real_distribution<double> real_dist(0.0, 1.0);
std::uniform_int_distribution<int> int_dist(1, SCREEN_WIDTH);

inline void time_passed(std::chrono::high_resolution_clock::time_point start, double &holder) {
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    holder = duration.count();
}

template <class T> inline void print_vector(const std::string &label, const std::vector<T> &vector) {
    std::cout << label << " = [ ";
    for (auto object : vector) {
        std::cout << object << ' ';
    }
    std::cout << "]\n";
}

inline nuworks::Way shuffle_way(const nuworks::Way &way) {
    nuworks::Way w(way);
    std::shuffle(w.nodes.begin(), w.nodes.end(), el);
    return w;
}

double nuworks::Node::distanceTo(const Node &node) const {
    return std::sqrt(std::pow(x - node.x, 2) + std::pow(y - node.y, 2));
}

void nuworks::Executor::generate_cities() {
    for (int i = 0; i < citiesCount; i++) {
        Node data{.id = i, .x = int_dist(el), .y = int_dist(el)};

        _cities.push_back(data);
    }
}

void nuworks::Executor::generate_lut() {
    for (int i = 0; i < citiesCount; i++) {
        auto a = _cities[i];

        for (int j = i + 1; j < citiesCount; j++) {
            auto b = _cities[j];
            auto data = a.distanceTo(b);
            auto key = std::make_pair(std::min(a.id, b.id), std::max(a.id, b.id));
            std::cout << a.id << " to " << b.id << " = " << data << std::endl;
            _dist_lut.insert(std::make_pair(key, data));
        }
    }
}

void nuworks::Executor::generate_first_population() {
    for (auto city : _cities) {
        _best_population.nodes.push_back(city.id);
    }
    _best_population.fitness = get_way_length(_best_population);
    _population.push_back(_best_population);
    for (int i = 1; i < populationCount; i++) {
        _population.push_back(shuffle_way(_best_population));
        _population[i].fitness = get_way_length(_population[i]);
    }
}

void nuworks::Executor::load_map() {
    std::string filename;
    std::cout << "Enter filename of map to open:";
    std::cin >> filename;

    std::ifstream in(filename);
    if (in.is_open()) {
        printf("Loading cities... ");
        double ellapsed_time_lc = 0;
        auto start_lc = std::chrono::high_resolution_clock::now();

        in >> citiesCount;
        _cities.resize(0);
        _cities.reserve(citiesCount);
        for (int i = 0; i < citiesCount; i++) {
            int x, y;
            in >> x >> y;
            Node city{.id = i, .x = x, .y = y};
            _cities.push_back(city);
        }
        in.close();
        time_passed(start_lc, ellapsed_time_lc);
        printf("done (%zu cities in %.2f microseconds)\n", _cities.size(), ellapsed_time_lc);

        printf("Generating LookUpTable... ");
        double ellapsed_time_glut = 0;
        auto start_glut = std::chrono::high_resolution_clock::now();
        generate_lut();
        time_passed(start_glut, ellapsed_time_glut);
        printf("done (%zu entries in %.2f microseconds)\n", _dist_lut.size(), ellapsed_time_glut);

        printf("Generating first population... ");
        double ellapsed_time_gfp = 0;
        auto start_gfp = std::chrono::high_resolution_clock::now();
        generate_first_population();
        time_passed(start_gfp, ellapsed_time_gfp);
        printf("done (%zu entries in %.2f microseconds)\n", _population.size(), ellapsed_time_gfp);
    } else {
        std::cerr << "Couldn't open file '" << filename << "'\n";
        exit(-1);
    }
}

void nuworks::Executor::write_map() {
    std::cout << "Writing graph data... ";

    std::string filename = "graph.dot";
    std::ofstream out(filename);
    if (out.is_open()) {
        out << "digraph {\n";
        for (auto node : _cities) {
            out << "\t" << node.id << " [";
            out << "\t\tlabel = " << node.id << "\n";
            out << "\t\tpos = \"" << node.x << "," << node.y << "!\"\n";
            out << "\t]\n";
        }

        for (int i = 1; i < citiesCount; i++) {
            out << "\n\t" << _best_population.nodes[i - 1] << " -> " << _best_population.nodes[i];
        }
        out << "\n}";

        std::cout << "done\n";
    } else {
        std::cout << "couldn't open file '" << filename << "'\n";
    }
}

void nuworks::Executor::init() {
    printf("Generating cities... ");
    double ellapsed_time_gc = 0;
    auto start_gc = std::chrono::high_resolution_clock::now();
    generate_cities();
    time_passed(start_gc, ellapsed_time_gc);
    printf("done (%zu cities in %.2f microseconds)\n", _cities.size(), ellapsed_time_gc);

    printf("Generating LookUpTable... ");
    double ellapsed_time_glut = 0;
    auto start_glut = std::chrono::high_resolution_clock::now();
    generate_lut();
    time_passed(start_glut, ellapsed_time_glut);
    printf("done (%zu entries in %.2f microseconds)\n", _dist_lut.size(), ellapsed_time_glut);

    printf("Generating first population... ");
    double ellapsed_time_gfp = 0;
    auto start_gfp = std::chrono::high_resolution_clock::now();
    generate_first_population();
    time_passed(start_gfp, ellapsed_time_gfp);
    printf("done (%zu entries in %.2f microseconds)\n", _population.size(), ellapsed_time_gfp);
}

double nuworks::Executor::get_way_length(const Way &way) {
    double out = 0.0;
    for (size_t i = 1; i < way.nodes.size(); i++) {
        auto a = _cities[way.nodes[i - 1]];
        auto b = _cities[way.nodes[i]];

        auto k = std::make_pair(std::min(a.id, b.id), std::max(a.id, b.id));
        out += _dist_lut[k];
    }
    return out;
}

void nuworks::Executor::calc_fitness() {
    for (int i = 0; i < _population.size(); i++) {
        _population[i].fitness = get_way_length(_population[i]);
        if (_population[i].fitness < _best_population.fitness) {
            _best_population.fitness = _population[i].fitness;
            _best_population.nodes = std::vector<int>(_population[i].nodes);
        }
    }
}

std::pair<int, int> nuworks::Executor::find_two_best() {
    double f = DBL_MAX;
    double s = DBL_MAX;
    int fi = -1;
    int fs = -1;

    for (int i = 0; i < _population.size(); i++) {
        if (_population[i].fitness < f) {
            s = f;
            fs = fi;
            f = _population[i].fitness;
            fi = i;
        } else if (_population[i].fitness < s) {
            s = _population[i].fitness;
            fs = i;
        }
    }

    return std::make_pair(fi, fs);
}

void nuworks::Executor::next_generation() {
    std::vector<Way> next_gen;
    next_gen.reserve(_population.size());

    auto top = find_two_best();
    auto first_best = _population[top.first];
    auto second_best = _population[top.second];
    while (next_gen.size() < populationCount) {
        auto crossed = cross_over(first_best, second_best);
        mutate(crossed.first);
        mutate(crossed.second);
        next_gen.push_back(crossed.first);
        next_gen.push_back(crossed.second);
    }
    if (next_gen.size() > populationCount) {
        next_gen.resize(populationCount);
    }
    _population.clear();
    _population = std::vector<Way>(next_gen);
    calc_fitness();
}

std::pair<nuworks::Way, nuworks::Way> nuworks::Executor::cross_over(const Way &p1, const Way &p2) {
    Way otf{};
    Way ots{};

    int i;
    for (i = 0; i < citiesCount / 2; i++) {
        otf.nodes.push_back(p1.nodes[i]);
        ots.nodes.push_back(p2.nodes[i]);
    }

    i = 0;
    while (otf.nodes.size() < citiesCount) {
        int v = p2.nodes[i++];
        if (std::find(otf.nodes.begin(), otf.nodes.end(), v) == otf.nodes.end()) {
            otf.nodes.push_back(v);
        }
    }

    i = 0;
    while (ots.nodes.size() < citiesCount) {
        int v = p1.nodes[i++];
        if (std::find(ots.nodes.begin(), ots.nodes.end(), v) == ots.nodes.end()) {
            ots.nodes.push_back(v);
        }
    }

    return std::make_pair(otf, ots);
}

void nuworks::Executor::mutate(nuworks::Way &w) {
    std::uniform_int_distribution<int> len_dist(0, w.nodes.size() - 1);
    for (size_t i = 0; i < w.nodes.size(); i++) {
        if (real_dist(el) > mutationChance) {
            int indexA = len_dist(el);
            int indexB = len_dist(el);

            std::iter_swap(w.nodes.begin() + indexA, w.nodes.begin() + indexB);
        }
    }
}

void nuworks::Executor::print_lut() {
    std::cout << "LUT: {\n";
    for (auto p : _dist_lut) {
        std::cout << '\t';
        std::cout << std::setfill(' ') << std::setw(2) << p.first.first << " to ";
        std::cout << std::setfill(' ') << std::setw(2) << p.first.second << " is " << p.second << "pts;\n";
    }
    std::cout << "} // LUT\n";
}

void nuworks::Executor::print_stats() {
    std::cout << "population: {\n";
    for (auto w : _population) {
        std::cout << "\tw: [ ";
        for (auto i : w.nodes) {
            std::cout << i << " ";
        }
        std::cout << "], " << w.fitness << " pts;\n";
    }
    std::cout << "} // population\n";

    std::cout << "best in current executor: [ ";
    for (auto i : _best_population.nodes) {
        std::cout << i << "(" << _cities[i].x << "," << _cities[i].y << ") ";
    }
    std::cout << "] with score " << _best_population.fitness << '\n';
}

int nuworks::Executor::getCitiesCount() { return this->citiesCount; }

int nuworks::Executor::getPopulationCount() { return this->populationCount; }
