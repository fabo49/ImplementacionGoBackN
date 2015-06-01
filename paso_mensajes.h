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
#include <QString>
#include <iostream>
#include <QDebug>
#include <queue>

#define LMA     1   //llega mensaje a A
#define SLA     2   //se libera A
#define SLB     3   //se libera B
#define BRF     4   //B recibe frame
#define ARACK   5   //A recibe ACK
#define VTTL    6   //vence time to live

struct frame{
    bool error;
    QString mensaje;
    int numSecuencia;
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

private:

    void correSimulacion();
    void A_recibeMensaje();
    void A_seLibera();
    void A_recibeACK();
    void expiraTTL();
    void B_recibeFrame();
    void B_seLibera();

    void sigEvento();

    Ui::Paso_Mensajes *ui;

    // Miembros de la clase
    int m_numVeces;
    double m_maxTime;
    double m_timer;
    bool m_modoLento;
    double m_reloj;

    double m_A_recibeMensaje;
    double m_A_seLibera;
    double m_A_recibeACK;
    double m_expiraTTL;
    double m_B_recibeFrame;
    double m_B_seLibera;

    std::queue<QString> colaA;
    std::queue<frame> colaB;

    QString ventanaMensajes[8];


};

#endif // PASO_MENSAJES_H
