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
    bool error;
    int numSecuencia;   //va a ser el mismo que le mensaje
};

struct mensaje{
    bool seEnvio;
    int numSecuencia;
    double venceTimer;
    double tiempoArrivo;
    double tiempoTransmision;
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
    void on_btnAbout_clicked();

signals:
    void avance(int progreso);
private:

    // ------------------------------------------
    // |        Eventos de la simulacion        |
    // ------------------------------------------

    void correSimulacion();
    void A_recibeMensaje();
    void A_seLibera();
    void A_recibeACK();
    void expiraTTL();
    void B_recibeFrame();
    void B_seLibera();

    // ----------------------------------
    // |        Métodos de ayuda        |
    // ----------------------------------

    int getMsjFaltante();
    void sigEvento();
    void clear();

    // --------------------------------------
    // |        Miembros de la clase        |
    // --------------------------------------

    Ui::Paso_Mensajes *ui;
    About *vAbout;


    // Variables que se le solicitan al usuario
    int m_numVeces;
    double m_maxTime;
    double m_timer;
    bool m_modoLento;

    // Tiempos de los eventos
    double m_A_recibeMensaje;
    double m_A_seLibera;
    double m_A_recibeACK;
    double m_expiraTTL;
    double m_B_recibeFrame;
    double m_B_seLibera;

    //Estructuras de datos
    std::vector<mensaje> colaA;
    std::vector<mensaje> ventana;
    std::vector<frame> colaB;
    std::vector<double> colaTimer;
    std::vector<int> tamColaA;
    std::vector<double> promTotalColaA;
    std::vector<double> permanencia;
    std::vector<double> promTotalPermanencia;
    std::vector<double> promTransmicion;
    std::vector<double> promTotalTransmicion;

    // Variables auxiliares
    bool A_Ocupado;
    bool B_Ocupado;
    int numMensajes;
    int mensajeActual;
    int ACK;

    // ------------------------------
    // |        Estadísticas        |
    // ------------------------------

    int totMensajesRecibidos;
    double m_reloj;
    double m_relojTotal;
    int m_totFramesB;
    int m_longColaA;
    int m_recibidosCorrectosB;
    QString datosGenerales;
    QString m_datosCPUB;
    QString m_datosCPUA;
};

#endif // PASO_MENSAJES_H
