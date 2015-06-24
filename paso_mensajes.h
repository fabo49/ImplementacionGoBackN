/**
 * Universidad de Costa Rica
 * Escuela de Ciencias de la Computación e Informática
 * Investigación de Operaciones
 * Profesora: Ileana Alpizar
 * Proyecto de simulación I: PROTOCOLO PARA ENVÍO DE MENSAJES EN UNA RED DE DOS COMPUTADORAS
 * @author Fabián Rodríguez Obando
 * @author Luis Diego Hernandez Herrera
 * I Semestre 2015
 */

#ifndef PASO_MENSAJES_H
#define PASO_MENSAJES_H

#include <QMainWindow>
#include <iostream>
#include <QDebug>
#include <vector>
#include <QThread>
#include <QString>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include <list>
#include <about.h>


struct frame{
    unsigned int numSecuencia;   //va a ser el mismo que el mensaje
};

struct mensaje{
    bool seEnvio;
    unsigned int numSecuencia;
    double tiempoArrivo;
};


namespace Ui {
class Paso_Mensajes;
}

class Paso_Mensajes : public QMainWindow
{
    Q_OBJECT

public:
    explicit Paso_Mensajes(QWidget *parent = 0);
    ~Paso_Mensajes();

private slots:
    void on_btnStart_clicked();
    void about();

signals:
    void avance(int progreso);
private:

    // ------------------------------------------
    // |        Eventos de la simulacion        |
    // ------------------------------------------

    void correSimulacion();
    void A_recibeMensaje();
    void A_seLibera();
    void B_recibeFrame();
    void B_seLibera();

    // ----------------------------------
    // |        Métodos de ayuda        |
    // ----------------------------------

    void sigEvento();
    void clear();
    void updateClock();
    int getMsjFaltante();

    // --------------------------------------
    // |        Miembros de la clase        |
    // --------------------------------------

    Ui::Paso_Mensajes *ui;
    About *vAbout;


    // Variables que se le solicitan al usuario
    int m_numVeces;
    double m_maxTime;
    bool m_modoLento;

    // Tiempos de los eventos
    double m_A_recibeMensaje;
    double m_A_seLibera;
    double m_B_recibeFrame;
    double m_B_seLibera;

    //Estructuras de datos
    std::vector<mensaje> colaA;
    std::vector<frame> colaB;
    std::vector<int> tamColaA;
    std::vector<double> promTotalColaA;
    std::vector<double> permanencia;
    std::vector<double> promTotalPermanencia;

    // Variables auxiliares
    bool A_Ocupado;
    bool B_Ocupado;
    unsigned int numMensajes;

    // ------------------------------
    // |        Estadísticas        |
    // ------------------------------

    double m_reloj;
    double m_relojTotal;
    QString datosGenerales;
    QString m_datosCPUA;
};

#endif // PASO_MENSAJES_H
