#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

#define POPULATION_SIZE 1000
#define MUTATION_RATE 0.1
#define ELITE_PERCENTAGE 0.3
#define RANDOM_PERCENTAGE 0.2

typedef std::string Chromosome;
typedef std::vector<Chromosome> Population;

const std::set<char> ALPHABET = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '+', '=', '{', '}', '[', ']', '|', '\\', ':', ';', '<', '>', ',', '.', '/', '?', ' '};

std::minstd_rand gen = std::minstd_rand{std::random_device{}()}; // Pseudo-random number generator
std::uniform_real_distribution<> percentage_dist{0.0, 1.0};      // Distribution for generating percentages

template <typename T = int>
T RandRange(T min, T max)
{
    return min + (percentage_dist(gen) * (max - min));
}

template <typename T = int>
T RandRange(T max)
{
    return RandRange<T>(0, max);
}

char RandomChar()
{
    int index = RandRange(ALPHABET.size());
    return *std::next(ALPHABET.begin(), index);
}

class GeneticPopulation
{
    std::string password;

public:
    Population *population;
    const Chromosome *found_password;

    int generation_num = 0;
    bool password_found = false;

    std::size_t chromosome_length;
    std::size_t population_size;
    float mutation_rate;
    std::size_t elite_num;
    std::size_t random_num;
    std::size_t selection_size = elite_num + random_num;

    GeneticPopulation(std::string password, int population_size, float mutation_rate, float elite_percentage, float random_percentage);

    Chromosome *GenerateChromosome();
    float EvaluateChromosome(const Chromosome &chromosome);
    Population *SelectChromosomes();
    Chromosome *Crossover(const Chromosome &parent1, const Chromosome &parent2);
    void MutateChromosome(Chromosome &chromosome);
    Population *NextGeneration();
    void Evolve();

    void PrintPopulation(int width, int height);
};

GeneticPopulation::GeneticPopulation(std::string password, int population_size = POPULATION_SIZE, float mutation_rate = MUTATION_RATE, float elite_percentage = ELITE_PERCENTAGE, float random_percentage = RANDOM_PERCENTAGE) : password(password), chromosome_length(password.length()), population_size(population_size), mutation_rate(mutation_rate), elite_num(population_size * elite_percentage), random_num(population_size * random_percentage)
{
    // Initialize the population
    this->population = new Population{};
    this->population->reserve(this->population_size);

    for (int i = 0; i < this->population_size; i++)
        this->population->push_back(*this->GenerateChromosome());
}

Chromosome *GeneticPopulation::GenerateChromosome()
{
    // Generate a random string
    Chromosome *chromosome = new Chromosome{};

    for (int i = 0; i < this->chromosome_length; i++)
        chromosome->push_back(RandomChar());

    return chromosome;
};

float GeneticPopulation::EvaluateChromosome(const Chromosome &chromosome)
{
    // Count the number of matching characters
    int match_count = 0;
    for (int i = 0; i < this->chromosome_length; i++)
        match_count += this->password[i] == chromosome[i];

    // Return a score between 0 and 1
    float score = (float)match_count / this->chromosome_length;

    // Check if the password is found
    if (score == 1.0)
    {
        this->password_found = true;
        this->found_password = &chromosome;
    }

    return score;
}

Population *GeneticPopulation::SelectChromosomes()
{
    // Generate score-chromosome map
    std::vector<std::pair<float, const Chromosome *>> score_chromosome_map{};
    score_chromosome_map.reserve(this->population_size);

    for (int i = 0; i < this->population_size; i++)
    {
        const Chromosome *chromosome = &this->population->at(i);

        float score = this->EvaluateChromosome(*chromosome);
        score_chromosome_map.emplace_back(score, chromosome);
    }

    // Sort the score-chromosome map according to score
    auto comparator = [](const std::pair<float, const Chromosome *> &a, const std::pair<float, const Chromosome *> &b)
    {
        return a.first > b.first;
    };
    std::sort(score_chromosome_map.begin(), score_chromosome_map.end(), comparator);

    Population *selection = new Population{};
    selection->reserve(this->selection_size);

    // Select the best portion of the population
    for (int i = 0; i < this->elite_num; i++)
    {
        Chromosome chromosome = *score_chromosome_map[i].second;
        selection->emplace_back(chromosome);
    }

    // Randomly select from the remaining population
    for (int i = 0; i < this->random_num; i++)
    {
        int index = RandRange(this->elite_num, this->population_size);
        Chromosome chromosome = *score_chromosome_map[index].second;
        selection->emplace_back(chromosome);
    }

    return selection;
};

Chromosome *GeneticPopulation::Crossover(const Chromosome &parent1, const Chromosome &parent2)
{
    // Combine the chromosomes using one-point crossover
    int index = this->chromosome_length / 2;

    std::string part_1 = std::string(parent1.begin(), parent1.begin() + index);
    std::string part_2 = std::string(parent2.begin() + index, parent2.end());
    Chromosome *child = new Chromosome{part_1 + part_2};

    return child;
};

void GeneticPopulation::MutateChromosome(Chromosome &chromosome)
{
    // Each character in the chromosome has a small chance to mutate
    for (char &c : chromosome)
        if (RandRange<float>(1) < this->mutation_rate)
            c = RandomChar();
};

Population *GeneticPopulation::NextGeneration()
{
    Population *new_population = this->SelectChromosomes();
    new_population->reserve(this->population_size - this->selection_size);

    // Fill the rest of the population with children
    for (int i = this->selection_size; i < this->population_size; i++)
    {
        Chromosome parent_1 = this->population->at(RandRange(this->selection_size));
        Chromosome parent_2 = this->population->at(RandRange(this->selection_size));

        Chromosome child = *this->Crossover(parent_1, parent_2);
        this->MutateChromosome(child);
        new_population->emplace_back(child);
    }

    this->generation_num++;

    return new_population;
};

void GeneticPopulation::Evolve()
{
    this->population = this->NextGeneration();
}

void GeneticPopulation::PrintPopulation(int width = 10, int height = 10)
{
    std::cout << "\x1b[u"; // Restore cursor position

    std::cout << "Generation: " << this->generation_num << '\n';

    // Print a portion of the population in a grid with the given dimensions
    for (int j = 0; j < height; j++)
    {
        for (int k = 0; k < width; k++)
            std::cout << this->population->at(width * j + k) << ' ';
        std::cout << '\n';
    }
}

void PrintWelcome() {
    std::cout << "##################################\n";
    std::cout << "#        Password Cracker        #\n";
    std::cout << "##################################\n";
    std::cout << "This simple example of a genetic algorithm was coded by Zac Hart.\n";
    std::cout << "You first enter a password, then the algorithm will attempt to guess it.\n";
    std::cout << "The only information it has access to is how close its guesses are.\n";
    std::cout << "You can see the first 100 guesses in a generation; once the password is found, the algorithm will stop.\n";
    std::cout << "Watch closely, because it doesn't take long!\n" << std::endl;
}

int main()
{
    PrintWelcome();

    // Get user input
    std::string password;
    std::cout << "Enter your password: ";
    std::cin >> password;
    std::cout << "\x1b[s"; // Save cursor position

    GeneticPopulation *gp = new GeneticPopulation(password);

    while (!gp->password_found)
    {
        gp->PrintPopulation(); // Print the current generation
        gp->Evolve();          // Evolve the population
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Delay for display purposes
    }

    std::cout << "\nPassword found! The password is: " << *gp->found_password << std::endl;

    delete gp;

    return 0;
}
