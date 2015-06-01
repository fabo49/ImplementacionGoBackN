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
    m_indiceVeces = 0;

    A_Ocupado = false;
    B_Ocupado = false;

    numMensajes = 0;
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
    ++m_indiceVeces;
    while(m_reloj < m_maxTime && m_indiceVeces < m_numVeces){
        sigEvento();
        ++m_indiceVeces;
    }
}

void Paso_Mensajes::A_recibeMensaje()
{
    m_reloj = m_A_recibeMensaje;    //R=LMA
    ++numMensajes;

    if(ventanaMensajes.size() == 8){    //la ventana esta llena
        colaA.push(numMensajes);        //guarda el mensaje en la cola
    }else{
        if(A_Ocupado == true){
            ventanaMensajes.push(numMensajes);
        }else{
            std::default_random_engine generador;
            std::exponential_distribution<double> distribucion(0.5);

            A_Ocupado = true;
            ventanaMensajes.push(numMensajes);
            mensajeActual = ventanaMensajes.front();
            ventanaMensajes.pop();
            double varAleatoria = distribucion(generador);  //crea la variable aleatoria con dist. exponencial, parametro 1/2
            m_A_seLibera = m_reloj + varAleatoria + 1;    //programa el evento Liberar A
        }
    }
    std::default_random_engine generador;
    std::normal_distribution<double> distribucion(25.0, 1.0);
    double varAleatoria = distribucion(generador);  //genera la variable aleatoria con dist. normal, media:25 desviacion estandar: 1
    m_A_recibeMensaje = m_reloj + varAleatoria;     //programa cuando va a arribar el siguiente mensaje
    QString reloj = QString::number(m_reloj);
    ui->labelReloj->setText(reloj);  //indica en la interfaz cual es el reloj
}

void Paso_Mensajes::A_seLibera()
{
    m_reloj = m_A_seLibera; //R = SLA
    m_B_recibeFrame = m_reloj + 1;
    //venceTimer = m_reloj + m_timer

    frame newFrame;
    newFrame.numSecuencia = mensajeActual;
    std::default_random_engine generador1;
    std::uniform_int_distribution<int> distribucion1(1,100);
    int numero = distribucion1(generador1);
    if(numero < 10){        //probabilidad 0.1 de que el mensaje vaya con error
        newFrame.error = true;
    }else{
        newFrame.error = false;
    }

    std::default_random_engine generador2;
    std::uniform_int_distribution<int> distribucion2(1, 100);
    numero = distribucion2(generador2);
    if(numero <5){      //probabilidad de 0.05 de que el mensaje se pierda

    }else{
        //programa el evento B_recibeFrame
        m_B_recibeFrame = m_reloj + 1;  //BRF = R+1
    }
    //luego hay que revisar si hay algo en cola
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
