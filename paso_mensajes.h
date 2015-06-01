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
#include "eventos.h"

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
    Ui::Paso_Mensajes *ui;

    // Miembros de la clase
    int m_numVeces;
    double m_maxTime;
    double m_timer;
    bool m_modoLento;
    double m_reloj;

};

#endif // PASO_MENSAJES_H
