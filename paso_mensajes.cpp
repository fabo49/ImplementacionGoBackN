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
    while(m_reloj < m_maxTime){
        int evento = minDouble(m_A_recibeMensaje, m_A_seLibera, m_B_seLibera, m_B_recibeFrame, m_A_recibeACK, m_expiraTTL);
        switch (evento){
        case LMA:
            A_recibeMensaje();
            break;
        case SLA:
            A_seLibera();
            break;
        case SLB:
            B_seLibera();
            break;
        case BRF:
            B_recibeFrame();
            break;
        case ARACK:
            A_recibeACK();
            break;
        case VTTL:
            expiraTTL();
            break;
        }
    }
}

void Paso_Mensajes::A_recibeMensaje()
{

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

int Paso_Mensajes::minDouble(double a, double b, double c,  double d, double e, double f)
{
    double vecTimes[] ={a, b, c, d, e, f};
    double min = a;
    for(int i=0; i<6; ++i){
        for(int j=i+1; j<6; ++j){
            if(vecTimes[j]<min){
                min = vecTimes[j];
            }
        }
    }

    if(min == a){
        return LMA;
    }
    if(min == b){
        return SLA;
    }
    if(min == c){
        return SLB;
    }
    if(min == d){
        return BRF;
    }
    if(min == e){
        return ARACK;
    }
    if(min == f){
        return VTTL;
    }

    return 0;
}
