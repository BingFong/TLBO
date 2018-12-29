/*Is RANDOM leagle?*/
#include <iostream>
#include <time.h>
#include <cmath>
using namespace std;

#define PS 50 //population size
#define maxCycle 500
#define D 13 //13 designed variables
#define LOWER_BOUND 0
#define UPPER_BOUND_1 1                          //i = 1,2,3,..,9
#define UPPER_BOUND_2 100                        //i = 10,11,12
#define UPPER_BOUND_3 1                          //i = 13
#define RANDOM (double)rand() / (RAND_MAX + 1.0) //[0,1)

int population[PS][D];
int teach_population[PS][D];
int learn_population[PS][D];
int f[PS]; //fitness of population
int f_teach[PS];
int f_learner[PS];
int globalMin[2] = {};  //global best obj value
int globalObjParams[D]; //global best obj params
int teacher[D];         //teacher
int mean[D];            //mean of population
int sum_var[D] = {};    //sumary of every designed variable
int fitness_sum = 0;

void calculateFitness(int index, int population[][D], int f[]);
int subject_to_constraints(int index, int population[][D]);

void learner()
{
    int fit = 0;

    for (int i = 0; i < PS; i++)
    {
        int threshold = 0;
        double R = RANDOM;
        int J = rand() % 50;
        if (i != J || i == 0)
        {
            if (f[i] < f[J])
            {
                for (int j = 0; j < D; j++)
                {
                    double x_new;
                    x_new = population[i][j] + R * (population[i][j] - population[J][j]);
                    learn_population[i][j] = round(x_new);
                }
            }
            else
            {
                for (int j = 0; j < D; j++)
                {
                    double x_new;
                    x_new = population[i][j] + R * (population[J][j] - population[i][j]);
                    learn_population[i][j] = round(x_new);
                }
            }

            threshold = subject_to_constraints(i, learn_population);
            if (threshold == 1)
            {
                calculateFitness(i, learn_population, f_learner);
            }
            else
                i--;
        }
        else
            i--;
    }

    for (int i = 0; i < PS; i++)
    {
        fit += f_learner[i];
    }

    if (fit < fitness_sum)
    {
        for (int i = 0; i < PS; i++)
        {
            for (int j = 0; j < D; j++)
            {
                population[i][j] = learn_population[i][j];
            }
            f[i] = f_learner[i];
        }
        fitness_sum = fit;
    }
}

int calculate_teaching_fitness(int index)
{
    f[index] = 0;

    f[index] += 5 * (teach_population[index][0] + teach_population[index][1] + teach_population[index][2] + teach_population[index][3]);

    f[index] -= 5 * (teach_population[index][0] * teach_population[index][0] + teach_population[index][1] * teach_population[index][1] + teach_population[index][2] * teach_population[index][2] + teach_population[index][3] * teach_population[index][3]);
    for (int i = 4; i < 13; i++)
    {
        f[index] -= teach_population[index][i];
    }
    return f[index];
}

void teaching()
{
    int fit = 0;
    for (int i = 0; i < PS; i++)
    {
        int threshold = 0;
        double R = RANDOM;
        double T = 1 + round(RANDOM);

        for (int j = 0; j < D; j++)
        {
            double x_new;
            x_new = population[i][j] + R * (teacher[j] - T * mean[j]);
            teach_population[i][j] = round(x_new);
        }

        threshold = subject_to_constraints(i, teach_population);
        if (threshold == 1)
        {
            calculateFitness(i, teach_population, f_teach);
        }
        else
        {
            i--;
        }
    }
    for (int i = 0; i < PS; i++)
    {
        fit += f_teach[i];
    }

    if (fit < fitness_sum)
    {
        for (int i = 0; i < PS; i++)
        {
            for (int j = 0; j < D; j++)
            {
                population[i][j] = teach_population[i][j];
            }
            f[i] = f_teach[i];
        }
        fitness_sum = fit;
    }
}

void iter_teacher()
{
    int iter_fitness = 0;
    int teacher_num;

    for (int i = 0; i < PS; i++)
    {
        if (f[i] < iter_fitness)
        {
            iter_fitness = f[i];
            teacher_num = i;
        }
    }
    for (int i = 0; i < D; i++)
    {
        teacher[i] = population[teacher_num][i];
    }
}

void calculate_mean()
{
    /*sumary  every designed variable*/
    for (int j = 0; j < PS; j++)
    {
        for (int k = 0; k < D; k++)
        {
            sum_var[k] += population[j][k];
        }
    }

    /*calculate mean*/
    for (int i = 0; i < D; i++)
    {
        mean[i] = sum_var[i] / PS;
        sum_var[i] = 0; // -> ZERO
    }
}
void memorize_best_solution()
{
    int best_solution_number, if_found = 0;

    /*find best fitness*/
    for (int j = 0; j < PS; j++)
    {
        if (f[j] < globalMin[0])
        {
            globalMin[0] = f[j];
            best_solution_number = j;
            if_found = 1;
        }
    }
    if (if_found == 1)
    {
        fitness_sum = 0;
        for (int i = 0; i < PS; i++)
        {
            fitness_sum += f[i];
        }
        for (int i = 0; i < D; i++)
        {
            globalObjParams[i] = population[best_solution_number][i];
        }
    }
}

void calculateFitness(int index, int population[][D], int f[])
{
    f[index] = 0;

    f[index] += 5 * (population[index][0] + population[index][1] + population[index][2] + population[index][3]);

    f[index] -= 5 * (population[index][0] * population[index][0] + population[index][1] * population[index][1] + population[index][2] * population[index][2] + population[index][3] * population[index][3]);
    for (int i = 4; i < 13; i++)
    {
        f[index] -= population[index][i];
    }
}

int subject_to_constraints(int index, int population[][D])
{
    for (int i = 0; i < 9; i++)
    {
        if (population[index][i] > 1 || population[index][i] < 0)
            return 0;
    }
    for (int i = 9; i < 12; i++)
    {
        if (population[index][i] > 100 || population[index][i] < 0)
            return 0;
    }
    if (population[index][12] > 1 || population[index][12] < 0)
        return 0;

    int Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9;
    Q1 = 2 * population[index][0] + 2 * population[index][1] + population[index][9] + population[index][10] - 10;
    if (Q1 > 0)
    {
        return 0;
    }
    Q2 = 2 * population[index][0] + 2 * population[index][2] + population[index][9] + population[index][11] - 10;
    if (Q2 > 0)
    {
        return 0;
    }
    Q3 = 2 * population[index][1] + 2 * population[index][2] + population[index][10] + population[index][11] - 10;
    if (Q3 > 0)
    {
        return 0;
    }
    Q4 = -8 * population[index][0] + population[index][9];
    if (Q4 > 0)
    {
        return 0;
    }
    Q5 = -8 * population[index][1] + population[index][10];
    if (Q5 > 0)
    {
        return 0;
    }
    Q6 = -8 * population[index][2] + population[index][11];
    if (Q6 > 0)
    {
        return 0;
    }
    Q7 = -2 * population[index][3] - population[index][4] + population[index][9];
    if (Q7 > 0)
    {
        return 0;
    }
    Q8 = -2 * population[index][5] - population[index][6] + population[index][10];
    if (Q8 > 0)
    {
        return 0;
    }
    Q9 = -2 * population[index][7] - population[index][8] + population[index][11];
    if (Q9 > 0)
    {
        return 0;
    }
    return 1;
}

/* randomly generate 13 designed variables*/
void init(int index)
{
    for (int i = 0; i < D; i++)
    {
        if (i < 9)
        {
            population[index][i] = rand() % (UPPER_BOUND_1 - LOWER_BOUND + 1) + LOWER_BOUND;
        }
        else if (i < 12)
        {
            population[index][i] = rand() % (UPPER_BOUND_2 - LOWER_BOUND + 1) + LOWER_BOUND;
        }
        else
        {
            population[index][i] = rand() % (UPPER_BOUND_3 - LOWER_BOUND + 1) + LOWER_BOUND;
        }
    }
}

void initialize()
{
    for (int i = 0; i < PS; i++)
    {
        int threshold = 0;
        init(i); /* randomly generate 13 designed variables*/
        threshold = subject_to_constraints(i, population);
        if (threshold == 1)
        {
            calculateFitness(i, population, f);
        }
        else
        {
            i--;
        }
    }
}

int main()
{
    srand(time(NULL));

    initialize();
    memorize_best_solution();

    for (int i = 0; i < maxCycle; i++)
    {
        /* teacher_phase();*/
        calculate_mean();
        iter_teacher();
        teaching();

        /*learner_phase();*/
        learner();
        memorize_best_solution();
    }
    cout << "global fit :" << globalMin[0] << endl;
    cout << "Params : ";
    for (int j = 0; j < D; j++)
    {
        cout << globalObjParams[j] << " , ";
    }
    cout << endl;

    system("PAUSE");
    return 0;
}