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
    m_reloj = 0;
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
//Se va empezar a implementar
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
