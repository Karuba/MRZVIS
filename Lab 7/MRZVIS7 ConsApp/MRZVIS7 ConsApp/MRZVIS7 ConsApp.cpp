#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <thread>
#include <mutex>

#define SIZE 10
#define SIZEGENES 8
#define SIZEPOP 20
#define MUTVER 20 // of 1000
#define ISOLPERIOD 5 // life in a isolation
#define COUNTOFHABITAT 3 // -- habitat num

using namespace std;
mutex unit_mutex, cout_mutex;
int a[SIZE][SIZE];

class Habitat {
public:
    int numarea;

    vector  <vector <int>> hrom;   

    class Animal {
    public:
        vector <int> genes;
        int adaptability;
        bool flag;
        bool migration;
        bool reproductive;
        Animal(vector <int> genes) {
            this->genes = genes;
            migration = 0;
            adaptability = findadapt();
            reproductive = 0;
            flag = 1;
        }
        int findadapt() {
            int start = 0, finish = SIZE - 1;
            int num = 0;
            num = a[0][genes[0]];
            for (int i = 0; i < genes.size() - 1; i++)
            {
                num += a[genes[i]][genes[i + 1]];
            }
            num += a[genes[genes.size() - 1]][SIZE - 1];
            return num;
        }
    };

    class ChildAnimal {
    public:
        vector <int> genes;
        int adaptability;
        bool reproductive;
        bool flag;
        ChildAnimal(vector <int> genes) {
            this->genes = genes;
            adaptability = findadapt();
            reproductive = 0;
            flag = 1;
        }
        int findadapt() {
            int start = 0, finish = SIZE - 1;
            int num = 0;
            num = a[0][genes[0]];
            for (int i = 0; i < genes.size() - 1; i++)
            {
                num += a[genes[i]][genes[i + 1]];
            }
            num += a[genes[genes.size() - 1]][SIZE - 1];
            return num;
        }
    };

    vector <Animal> population;

    Habitat(vector <int> begingen, int num) {
        numarea = num;
        hrom.push_back(vector <int>());
        hrom[0] = begingen;

        startpop();
        for (int i = 0; i < SIZEPOP; i++)
        {
            population.push_back(Animal(hrom[i]));
        }
    }

    void startpop() {        
        for (int i = 1; i < SIZEPOP; i++)
        {
            hrom.push_back(vector <int>());
            hrom[i] = hrom[i - 1];
            next_permutation(hrom[i].begin(), hrom[i].end());
        }
    }

    void roulette_wheel_selection(vector <Animal>& population) {
        vector <int> adapts;
        for (int i = 0; i < population.size(); i++)
        {
            adapts.push_back(population[i].adaptability);
        }

        int max = *max_element(adapts.begin(), adapts.end()) + 5;

        for (int i = 0; i < adapts.size(); i++)
        {
            adapts[i] = max - adapts[i];
        }

        int sum = 0;
        for (int i = 0; i < adapts.size(); i++)
        {
            sum += adapts[i];
        }

        random_device random_device;
        mt19937 generator(random_device());
        uniform_int_distribution<> distribution(0, sum);


        for (int i = 0; i < SIZEPOP; )
        {
            int randnum = distribution(generator);
            int asum = 0;
            for (int j = 0; j < adapts.size(); j++)
            {
                asum += adapts[j];
                if (randnum <= asum && !population[j].reproductive)
                {
                    population[j].reproductive = 1;
                    i++;
                    break;
                }
            }
        }
    }

    void intermediate_recombination(vector <Animal>& population, vector <ChildAnimal>& childpopulation) {
        double d = 0.25;
        random_device random_device;
        mt19937 generator(random_device());
        uniform_int_distribution<> distribution(-d, 1 + d);
        uniform_int_distribution<> MutVer(0, 1000);
        uniform_int_distribution<> MutGen(1, 8);
        for (int i = 0; i < population.size() - 1; i++)
        {
            for (int j = i + 1; j < population.size(); j++)
            {
                vector <int> genes;
                for (int t = 0; t < population[i].genes.size(); t++)
                {
                    double a = distribution(generator);

                    if (MutVer(generator) <= MUTVER)
                    {
                        genes.push_back(MutGen(generator) % SIZE);
                    }
                    else
                    {
                        int x = population[i].genes[t] + a * (population[j].genes[t] - population[i].genes[t]);
                        genes.push_back(x % SIZE);
                    }
                }
                childpopulation.push_back(ChildAnimal(genes));
            }
        }
    }

    void elite_selection(vector <Animal>& population, vector <ChildAnimal>& childpopulation) {
        for (int i = 0; i < SIZEPOP; i++)
        {
            int elit = 999, it; bool pl;
            for (int j = 0; j < population.size(); j++)
            {
                if (population[j].flag && population[j].adaptability < elit)
                {
                    elit = population[j].adaptability;
                    it = j;
                    pl = 1;
                }
            }
            for (int j = 0; j < childpopulation.size(); j++)
            {
                if (childpopulation[j].flag && childpopulation[j].adaptability < elit)
                {
                    elit = childpopulation[j].adaptability;
                    it = j;
                    pl = 0;
                }
            }
            if (pl)
            {
                population[it].flag = 0;
            }
            else
            {
                childpopulation[it].flag = 0;
            }
        }
        /*int num = 0;
        for (int i = 0; i < population.size(); i++)
        {
            if (!population[i].flag)
            {
                num++;
                cout << 1 << population[i].adaptability << endl;
            }
        }
        for (int i = 0; i < childpopulation.size(); i++)
        {
            if (!childpopulation[i].flag)
            {
                num++;
                cout << 2 << childpopulation[i].adaptability << endl;
            }
        }
        cout << endl << num;*/

        int it = 0;
        for (int i = 0; i < population.size(); i++)
        {
            for (int j = i; j < population.size(); j++)
            {
                if (!population[j].flag)
                {
                    population[i].genes = population[j].genes;
                    population[i].adaptability = population[j].adaptability;
                    population[j].flag = 1;
                    population[i].flag = 1;
                    population[i].reproductive = 0;
                    it = i;
                    break;
                }
            }
        }
        for (int i = it + 1; i < population.size(); i++)
        {
            for (int j = i; j < childpopulation.size(); j++)
            {
                if (!childpopulation[j].flag)
                {
                    population[i].genes = childpopulation[j].genes;
                    population[i].adaptability = childpopulation[j].adaptability;
                    childpopulation[j].flag = 1;
                    population[i].flag = 1;
                    population[i].reproductive = 0;
                    break;
                }
            }
        }
    }

    void start_evol_loop() {       

        //for (int i = 0; i < population.size(); i++)
        //{
        //    for (int j = 0; j < population[i].genes.size(); j++)
        //    {
        //        cout << population[i].genes[j] << " -- ";
        //    }
        //    cout << population[i].adaptability << endl;
        //}
        //cout << endl;
        int it = 0;
        while (it < ISOLPERIOD)
        {
            it++;
            //roulette_wheel_selection(population);
            vector <ChildAnimal> childpopulation;
            intermediate_recombination(population, childpopulation);
            elite_selection(population, childpopulation);
        //    cout << "================<" << numarea << ">================" << endl;
        //    cout << "================ " << it << " ================" << endl;
        //    for (int i = 0; i < population.size(); i++)
        //    {
        //        for (int j = 0; j < population[i].genes.size(); j++)
        //        {
        //            cout << population[i].genes[j] << " -- ";
        //        }
        //
        //        cout << population[i].adaptability << endl;
        //    }
        //    cout << endl;
            //system("pause");
        }
    }

    void showpopulation() {
        cout << "================<" << numarea << ">================" << endl;
        for (int i = 0; i < population.size(); i++)
        {
            for (int j = 0; j < population[i].genes.size(); j++)
            {
                cout << population[i].genes[j] << " -- ";
            }
            if (population[i].migration)
            {
                cout << population[i].adaptability << " <--- M!" <<endl;
            }
            else
                cout << population[i].adaptability << endl;
        }
        cout << endl;
    }
};

///////////////
vector <Habitat> habitats;
///////////////

void write(string file) {
    ifstream fin;
    fin.open(file);
    if (fin.is_open())
    {
        int ch; bool check = 1; int i = 0, j = 0;
        while (fin >> ch)
        {
            if (ch == SIZE && check)
            {
                fin >> ch;
                check = 0;
            }
            if (!check)
            {
                if (j < SIZE)
                {
                    a[i][j] = ch;
                    j++;
                }
                else {
                    i++;
                    j = 0;
                    a[i][j] = ch;
                    j++;
                }
            }
            else
            {
                cout << "Error size";
                fin.close();
                return;
            }
        }
        fin.close();
        return;
    }
    else {
        cout << "Error open";
        return;
    }
}

//Help function
long double fact(int N)
{
    if (N < 0) 
        return 0;
    if (N == 0) 
        return 1;
    else 
        return N * fact(N - 1); 
}
///////////////

//Functions for threadings
void life_loop(Habitat habitat, int i);
void migration();
void isol_life() {    
    vector<thread> thread_group;
    for (int i = 0; i < habitats.size(); ++i)
    {
        thread_group.push_back(move(thread(life_loop, habitats[i], i)));
    }
    for (auto it = thread_group.begin(); it != thread_group.end(); ++it)
    {
        it->join();
    }
    cout << "After thread: \n\n";
    for (int i = 0; i < habitats.size(); i++)
    {
        habitats[i].showpopulation();
    }
    migration();
    cout << "After migration: \n\n";
    for (int i = 0; i < habitats.size(); i++)
    {
        habitats[i].showpopulation();
    }
    for (int i = 0; i < habitats.size(); i++)
    {
        for (int j = 0; j < habitats[i].population.size(); j++)
        {
            if (habitats[i].population[j].migration)
            {
                habitats[i].population[j].migration = 0;
                break;
            }
        }
    }
}
void life_loop(Habitat habitat, int i) {
    habitat.start_evol_loop();
    habitats[i].population = habitat.population;
}

void migration() {
    for (int i = 0; i < habitats.size(); i++)
    {
        int max = 0;
        for (int j = 0; j < habitats[i].population.size(); j++)
        {
            if (habitats[i].population[j].adaptability > max)
            {
                max = habitats[i].population[j].adaptability;
            }
        }
        for (int j = 0; j < habitats[i].population.size(); j++)
        {
            if (habitats[i].population[j].adaptability == max)
            {
                habitats[i].population[j].migration = 1;
                break;
            }
        }
    }
    for (int i = 0; i < habitats.size() - 1; i++)
    {
        for (int j = 0; j < habitats[i].population.size(); j++)
        {
            if (habitats[i].population[j].migration)
            {
                for (int t = 0; t < habitats[i+1].population.size(); t++)
                {
                    if (habitats[i + 1].population[t].migration)
                    {
                        vector <int> tempv = habitats[i].population[j].genes;
                        int tempa = habitats[i].population[j].adaptability;

                        habitats[i].population[j].genes = habitats[i + 1].population[t].genes;
                        habitats[i].population[j].adaptability = habitats[i + 1].population[t].adaptability;

                        habitats[i + 1].population[t].genes = tempv;
                        habitats[i + 1].population[t].adaptability = tempa;
                        break;
                    }
                }
                break;
            }
        }
    }    
}
////////////////////////////

int main()
{
    string file = "Graf.txt";
    write(file);
    vector <int> begingen = { 1,2,3,4,5,6,7,8 }, gen = begingen;
    random_device random_device;
    mt19937 generator(random_device());
    uniform_int_distribution<> distribution(SIZEPOP, fact(SIZEGENES));
    
    for (int i = 0; i < COUNTOFHABITAT; i++)
    {
        for (int j = 0; j < distribution(generator); j++)
        {
            next_permutation(gen.begin(),gen.end());
        }
        if (i%2)
        {
            int tmp = gen[0];
            gen[0] = gen[gen.size() - 1];
            gen[gen.size() - 1] = tmp;
        }
        habitats.push_back(Habitat(gen, i + 1));
    }

    cout << "Before thread: \n\n";
    for (int i = 0; i < habitats.size(); i++)
    {
        habitats[i].showpopulation();
    }
    while (true)
    {
        isol_life();
        system("pause");
    }
}