#include "executor.h"

#include <iostream>
#include <omp.h>

void full_auto() {
    int citiesCount, populationCount, trainingSteps;
    std::cout << "citiesCount=";
    std::cin >> citiesCount;
    std::cout << "populationCount=";
    std::cin >> populationCount;
    std::cout << "trainingSteps=";
    std::cin >> trainingSteps;

    nuworks::Executor e(citiesCount, populationCount);
    e.init();
    e.print_lut();
    e.print_stats();

    for (int i = 1; i <= trainingSteps; i++) {
        std::cout << "Generation " << i << '\n';
        e.calc_fitness();
        e.next_generation();
        if (e.getCitiesCount() <= 10 && e.getPopulationCount() <= 10) {
            e.print_stats();
        }
        std::cout << "=========================================\n\n";
    }

    std::cout << "Final stats after " << trainingSteps << " generations\n";
    e.print_stats();
    e.write_map();
}

void manual_control() {
    int populationCount, trainingSteps;
    std::cout << "populationCount=";
    std::cin >> populationCount;
    std::cout << "trainingSteps=";
    std::cin >> trainingSteps;

    nuworks::Executor e(0, populationCount);
    e.load_map();
    e.print_lut();
    e.print_stats();

    for (int i = 1; i <= trainingSteps; i++) {
        std::cout << "Generation " << i << '\n';
        e.calc_fitness();
        e.next_generation();
        if (e.getCitiesCount() <= 10 && e.getPopulationCount() <= 10) {
            e.print_stats();
        }
        std::cout << "=========================================\n\n";
    }

    std::cout << "Final stats after " << trainingSteps << " generations\n";
    e.print_stats();
    e.write_map();
}

int main() {
    do {
        std::cout << "Menu\n\t0. Exit\n----------\n\t1. Automatic mode\n\t2. Manual mode\n>>>";
        int mode = 0;
        std::cin >> mode;
        switch (mode) {
        case 0:
            goto exit;
        case 1:
            full_auto();
            break;
        case 2:
            manual_control();
            break;
        }
    } while (true);

exit:
    return 0;
}
