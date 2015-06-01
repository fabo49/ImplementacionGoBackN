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

#include "paso_mensajes.h"
#include "ui_paso_mensajes.h"

Paso_Mensajes::Paso_Mensajes(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Paso_Mensajes)
{
    ui->setupUi(this);
    m_numVeces = 0;
    m_maxTime = 0.0;
    m_timer = 0.0;
    m_modoLento = false;
    m_reloj = 0.0;
}

Paso_Mensajes::~Paso_Mensajes()
{
    delete ui;
}

void Paso_Mensajes::on_btnStart_clicked()
{
    QString tempStr;
    tempStr = ui->lineNumVeces->text();
    bool ok;
    if(tempStr.isEmpty() == false){
        m_numVeces = tempStr.toInt(&ok, 10);
        tempStr.clear();
        tempStr = ui->lineMaxTime->text();
        if(tempStr.isEmpty() == false){
            m_maxTime = tempStr.toDouble(&ok);
            tempStr.clear();
            tempStr = ui->lineTimer->text();
            if(tempStr.isEmpty() == false){
                //llega aca significa que los campos necesarios estaban llenos
                m_timer = tempStr.toDouble(&ok);
                m_modoLento = ui->checkBox->isChecked();
                ui->checkBox->setEnabled(false);
                ui->lineMaxTime->setEnabled(false);
                ui->lineTimer->setEnabled(false);
                ui->lineNumVeces->setEnabled(false);
                ui->resultadosFinales->setText("Los datos necesarios estan llenos, vamos a empezar la simulación.");
                correSimulacion();
            }else{
                ui->resultadosFinales->setText("No se asignó un timer.");
            }
        }else{
            ui->resultadosFinales->setText("No se asignó un máximo de tiempo.");
        }
    }else{
        ui->resultadosFinales->setText("No se asigno el número de veces a correr.");
    }
}

void Paso_Mensajes::correSimulacion()
{
    A_recibeMensaje();
    while(m_reloj < m_maxTime){
        sigEvento();
    }
}

void Paso_Mensajes::A_recibeMensaje()
{
    m_reloj = m_A_recibeMensaje;

}

void Paso_Mensajes::A_seLibera()
{

}

void Paso_Mensajes::A_recibeACK()
{

}

void Paso_Mensajes::expiraTTL()
{

}

void Paso_Mensajes::B_recibeFrame()
{

}

void Paso_Mensajes::B_seLibera()
{

}

void Paso_Mensajes::sigEvento()
{
    double vecTimes[] ={m_A_recibeMensaje, m_A_seLibera, m_B_seLibera, m_B_recibeFrame, m_A_recibeACK, m_expiraTTL};
    double min = m_A_recibeMensaje;
    for(int i=0; i<6; ++i){
        for(int j=i+1; j<6; ++j){
            if(vecTimes[j]<min){
                min = vecTimes[j];
            }
        }
    }

    if(min == m_A_recibeMensaje){
        A_recibeMensaje();
    }
    if(min == m_A_seLibera){
        A_seLibera();
    }
    if(min == m_B_seLibera){
        B_seLibera();
    }
    if(min == m_B_recibeFrame){
        B_recibeFrame();
    }
    if(min == m_A_recibeACK){
        A_recibeACK();
    }
    if(min == m_expiraTTL){
        expiraTTL();
    }

}
