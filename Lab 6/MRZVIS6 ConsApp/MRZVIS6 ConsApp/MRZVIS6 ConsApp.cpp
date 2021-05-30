#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>

#define SIZE 10 // -- size for weight matrix for calculating adaptability || DON'T CHANGE !!!
#define SIZEGENES 8 // -- genes size (without first & last chromosomal of genes)  || DON'T CHANGE !!!
#define SIZEPOP 20 // -- population size
#define MUTVER 20 // of 1000 -- mutation probability

using namespace std;

vector  <vector <int>> hrom;
int a[SIZE][SIZE]; // weight matrix

class Animal {
public:
    vector <int> genes;
    int adaptability;
    bool flag;
    bool life;
    bool reproductive;
    Animal(vector <int> genes) {
        this->genes = genes;
        life = 1;
        adaptability = findadapt();
        reproductive = 0;
        flag = 1;
    }
    int findadapt() {
        int start = 0, finish = SIZE-1;
        int num = 0;
        num = a[0][genes[0]];
        for (int i = 0; i < genes.size()-1; i++)
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
    bool life;
    bool reproductive;
    bool flag;
    ChildAnimal(vector <int> genes) {
        this->genes = genes;
        life = 1;
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

void startpop() {   
    hrom.push_back(vector <int>());
    hrom[0] = { 1,2,3,4,5,6,7,8 };
    for (int i = 1; i < SIZEPOP; i++)
    {        
        hrom.push_back(vector <int>());
        hrom[i] = hrom[i - 1];
        next_permutation(hrom[i].begin(), hrom[i].end());
    }
}

void roulette_wheel_selection(vector <Animal> &population) {
    vector <int> adapts;
    for (int i = 0; i < population.size(); i++)
    {
        adapts.push_back(population[i].adaptability);
    }

    int max = *max_element(adapts.begin(),adapts.end()) + 5;
    
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

void intermediate_recombination(vector <Animal>& population, vector <ChildAnimal> &childpopulation) {
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
            if (population[j].flag && population[j].adaptability < elit )
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

int main()
{
    string file = "Graf.txt";
    write(file);
    vector <Animal> population;
    startpop();
    for (int i = 0; i < SIZEPOP; i++)
    {
        population.push_back(Animal(hrom[i]));
    }
    cout << "================ initial population ================" << endl;
    cout << "               chromosomals                 adaptability" << endl;
    for (int i = 0; i < population.size(); i++)
    {
        for (int j = 0; j < population[i].genes.size(); j++)
        {
            cout << population[i].genes[j] << " -- ";
        }
        cout << "\t" << population[i].adaptability << endl;
    }
    cout << endl;
    system("pause");
    int it = 0;
    while (true)
    {
        it++;
        //roulette_wheel_selection(population);
        vector <ChildAnimal> childpopulation;
        intermediate_recombination(population, childpopulation);
        elite_selection(population, childpopulation);
        cout << "================ " << it << " ================" << endl;
        cout << "               chromosomals                 adaptability" << endl;
        for (int i = 0; i < population.size(); i++)
        {
            for (int j = 0; j < population[i].genes.size(); j++)
            {
                cout << population[i].genes[j] << " -- ";
            }

            cout << "\t" << population[i].adaptability << endl;
        }
        cout << endl;
        //system("pause");
    }
}