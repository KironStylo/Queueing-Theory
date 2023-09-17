#include <cstdlib>
#include <iostream>
#include <time.h>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <math.h>



class QT
{
private:
    float TEAmean; // Variable aleatorio entre tiempos de arribo
    float STmean; // Variable aleatoria tiempo de servicio
    int N; // Cantidad de piezas
    std::vector<std::vector<float>> matrix;
    std::vector<std::string> column_names;

    // Funcion para generar números aleatorios para las dos variables aleatorias
    std::vector<float> generateRandomNumber(std::vector<float> rows);
    // Funcion para determinar el promedio de cada columna en la matriz
    std::vector<float> obtainColAverages();
    // Funcion para la suma total de columnas por pieza
    float totalSum(float);

public:
    QT(int, int, float, float);
    // Realiza los calculos de teoria de colas
    void fillMatrix();
    // Muestra una tabla comparativa de las medidas empíricas y teóricas
    void showComparison();
    // Retorna una impresión tabular de las 500 filas cada una con sus 8 valores
    void printMatrix();
};



QT::QT(int n, int m, float TEAmean, float STmean)
{
    // Se inicializa el vector con n elementos y cada elemento se le inicializa un vector de m elementos
    std::vector<std::vector<float>> matrix(n,std::vector<float>(m,100000));
    std::copy(matrix.begin(),matrix.end(),std::inserter(this->matrix,this->matrix.begin()));
    // Se colocan los titulos de cada columna
    std::vector<std::string> column_names
    {
        " --- TEA --- ",
        " --- TS --- ",
        " --- HA --- ",
        " --- IS --- ",
        " --- TC --- ",
        " --- TOS --- ",
        " --- FS --- ",
        " --- TES --- "
    };
    std::copy(column_names.begin(),column_names.end(),std::inserter(this->column_names, this->column_names.begin()));
    // Se asignan los valores de media
    this->TEAmean = TEAmean;
    this->STmean = STmean;
    this->N = n;
}


// Función para generar los numeros aleatorios para cada variable
std::vector<float> QT:: generateRandomNumber(std::vector<float> rows)
{
    // Variable aleatoria entre tiempos de arribo
    rows[0] = -(this->TEAmean)*std::log(1-(float)rand()/(float)RAND_MAX);
    // Variable aleatorio tiempo de servicio
    rows[1] = -(this->STmean)*std::log(1-(float)rand()/(float)RAND_MAX);

    return rows;
}

float QT::totalSum(float number)
{
    return number*this->matrix.size();
}

// Se llena la matriz con datos aleatorios y calculos de teoria de colas por columna
void QT::fillMatrix()
{
    srand(1);

    // Se asignan valores a las casillas de la matriz aplicando la formula respectiva
    std::vector<std::vector<float>> matrix;
    matrix.resize(this->matrix.size());
    // Se aplica la programación funcional para generar valores aleatorios.
    std::transform(this->matrix.begin(),this->matrix.end(),matrix.begin(),
                   [this](std::vector<float> rows)
    {
        return generateRandomNumber(rows);
    });
    this->matrix  = matrix;


    // Se recorre la matriz fila por fila
    for(int j = 0; j < this->matrix.size(); j++)
    {
        for(int u = 2; u < this->matrix[0].size(); u++)
        {
            // Se ignoran las dos primeras columnas puesto que estas son generadas de manera aleatoria.
            switch(u+1)
            {
            // Columna de hora de arribo 2
            case 3:
            {
                if(j == 0)
                {
                    this->matrix[j][u] = this->matrix[j][0];
                }
                else
                {
                    // Suma de TEA y HA anterior
                    this->matrix[j][u] = this->matrix[j][0]+this->matrix[j-1][2];
                }
                break;
            }
            // Columna de inicio de servicio 3
            case 4:
            {
                if(j == 0)
                {
                    this->matrix[j][u] = this->matrix[j][1];
                }
                else
                {
                    // Máximo entre FS anterior y HA actual
                    this->matrix[j][u] = std::max(this->matrix[j-1][6],this->matrix[j][2]);
                }
                break;
            }
            // Columna de tiempo en cola 4
            case 5:
            {
                if(j == 0)
                {
                    this->matrix[j][u] = 0;
                }
                else
                {
                    // Diferencia entre inicio de servicio y hora de arribo
                    this->matrix[j][u] = this->matrix[j][3]-this->matrix[j][2];
                }
                break;
            }
            // Columna de tiempo ocio servidor 5
            case 6:
            {
                if(j == 0)
                {
                    this->matrix[j][u] = this->matrix[j][2];
                }
                else
                {
                    // Diferencia entre inicio de servicio y fin de servicio anterior
                    this->matrix[j][u] = this->matrix[j][3]-this->matrix[j-1][6];
                }
                break;
            }
            // Se aplica la misma formula para cuando la fila es o no la primera tanto en la columna 7 como 8

            // Columna fin de servicio 6
            case 7:
            {
                // Suma de tiempo de servicio e inicio de servicio
                this->matrix[j][u] = this->matrix[j][1]+this->matrix[j][3];
                break;
            }
            // Columna hora de arribo 7
            case 8:
            {
                // Diferencia entre fin de servicio y hora de arribo
                this->matrix[j][u] = this->matrix[j][6]-this->matrix[j][2];
                break;
            }
            }
        }
    }

    std::vector<float> averages = obtainColAverages();
    std::vector<float> total_sum = averages;
    // Función lambda para obtener la suma
    std::transform(total_sum.begin(),total_sum.end(),total_sum.begin(),[this](float n)
    {
        return totalSum(n);
    });
    this->matrix.insert(this->matrix.end(), {total_sum,averages});
}

// Funcion que retorna el promedio de cada columna
std::vector<float> QT::obtainColAverages()
{
    std::vector<float> averages(8);
    std::vector<std::vector<float>> transposed(8,std::vector<float>(500));
    // Se halla la matriz  transpuesta de modo que se calcule el promedio de cada columna
    for(int j =0; j <this->matrix[0].size(); j++)
    {
        for(int i = 0; i < this->matrix.size(); i++)
        {
            transposed[j][i] = this->matrix[i][j];
            averages[j]+=transposed[j][i]/this->matrix.size();
        }
    }
    return averages;
}


void QT::printMatrix()
{
    int spacing, last_spacing = 0;
    bool first_time = true;

    int space_count = 0;

    std::stringstream ss;

    for(auto h : this->column_names)
    {
        spacing = 6;
        if(first_time)
        {
            std::cout << std::setw(spacing+12) << h;
            first_time = false;
        }
        else
        {
            std::cout << std::setw(last_spacing) << h;
        }
        last_spacing = spacing+10;
        ss << h;
        space_count += (ss.str().length()+4);
        ss.str("");
    }

    std::cout << std::endl;
    for(int n = 0; n <  space_count; n++)
    {
        std::cout << "_";
    }
    std::cout << std::endl;

    // Cuenta el indice que lleva la fila
    int index = 1;
    for(auto i: this->matrix)
    {
        first_time = true;
        last_spacing = 0;
        if(i == this->matrix[this->matrix.size()-1])
        {
            std::cout << std::setw(space_count/2) << "Promedios" << std::endl;
        }
        else if(i == this->matrix[this->matrix.size()-2])
        {
            std::cout << std::setw(space_count/2) << "Suma total" << std::endl;
        }
        for(auto j : i)
        {

            if(first_time)
            {
                std::cout << std::setw(3)<< index <<std::setw(spacing+8) << j << " |";
                first_time = false;
            }
            else
            {
                std::cout << std::setw(last_spacing) << j << " |";
            }
            last_spacing = spacing+8;
        }
        std::cout << std::endl;
        for(int n = 0; n <  space_count; n++)
        {
            std::cout << "_";
        }
        std::cout << std::endl;

        index++;
    }
}


void QT::showComparison()
{
    std::vector<std::vector<float>> comparison_matrix(2,std::vector<float>(6));

    // Encontrar valores empiricos
    float FS_final = this->matrix[this->matrix.size()-3][6];
    float TC_suma = this->matrix[this->matrix.size()-2][4];
    float TES_suma = this->matrix[this->matrix.size()-2][7];


    comparison_matrix[0][0] = TES_suma/this->N;
    comparison_matrix[0][1] = TC_suma/this->N;
    comparison_matrix[0][2] = comparison_matrix[0][0]-comparison_matrix[0][1];
    comparison_matrix[0][3] = TES_suma/FS_final;
    comparison_matrix[0][4] = TC_suma/FS_final;
    comparison_matrix[0][5] = comparison_matrix[0][3]-comparison_matrix[0][4];

    // Encontrar valores teóricos
    float lambda = 1/this->TEAmean;
    float miu = 1/this->STmean;

    comparison_matrix[1][0] = (1/(miu-lambda));
    comparison_matrix[1][1] = (lambda/(miu*(miu-lambda)));
    comparison_matrix[1][2] = comparison_matrix[1][0]-comparison_matrix[1][1];
    comparison_matrix[1][3] = (lambda/(miu-lambda));
    comparison_matrix[1][4] = ((lambda*lambda)/(miu*(miu-lambda)));
    comparison_matrix[1][5] = comparison_matrix[1][3]-comparison_matrix[1][4];

    std::vector<std::string> row_name {"W","Wq","Ws","L","Lq","Ls"};

    int title_spacing = 8;
    // Imprimir los valores
    std::cout << std::setw(title_spacing+18) << "Valores Empiricos" << std::setw(title_spacing+27) << "Valores Teoricos";

    int space_count = 64;
    std::cout << std::endl;
    for(int n = 0; n < 64 ; n++)
    {
        std::cout << "_";
    }
    std::cout << std::endl;


    for(int n = 0; n < 6; n++)
    {
        std::cout << std::setw(3) <<row_name[n]<<"|";
        title_spacing = 8;

        for(int i = 0; i < 2; i++)
        {
            title_spacing+=14;
            std::cout << std::setw(title_spacing) << comparison_matrix[i][n] <<"|";
        }
        std::cout << std::endl;
        for(int n = 0; n < 64 ; n++)
        {
            std::cout << "_";
        }
        std::cout << std::endl;
    }


}



/*
    La matriz esta compuesta por las siguientes columnas

    TEA - TS - HA - IS - TC - TOS - FS - TES
    0     1    2    3    4    5     6    7
    Donde,

    TEA -> Tiempo entre arribos.
    TS -> Tiempo de servicio.
    HA -> Hora de arribo.
    IS -> Inicio de servicio.
    TC -> Tiempo en cola.
    TOS -> Tiempo ocio servidor.
    FS -> Fin de servicio
    TES -> Tiempo en sistema
*/

/*
    La matriz de comparación esta compuesta por las siguientes columnas
        Valores  Empiricos              Valores Teóricos
                0                               1
    La matriz de comparación está compuesta por las siguientes filas
        0  W  -> Tiempo promedio de espera
        1  Wq -> Tiempo promedio de espera en la fila
        2  Ws -> Tiempo promedio de espera en el servicio
        3  L  -> Número promedio de piezas
        4  Lq -> Numero promedio de piezas en la fila
        5  Ls -> Número promedio de piezas en el servicio
*/

void repeticiones(){
    // Se instancia una clase de QT
    QT *qt = new QT(500,8,15.0,10.0);

    // Se llena la matriz
    qt->fillMatrix();

    // Se imprime la matriz
    qt->printMatrix();

    // Se realizan los calculos
    qt->showComparison();

    // Se limpia de la memoria el objeto
    delete qt;
}
int main()
{
    int t = 100;
    while(t--){
        std::cout << "Iteración " << (100-t+1) << std::endl;
        repeticiones();
    }
}
