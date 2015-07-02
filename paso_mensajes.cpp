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
    m_modoLento = false;
    m_reloj = 0.0;
    m_relojTotal = 0.0;


    A_Ocupado = false;
    B_Ocupado = false;

    numMensajes = 1;

    datosGenerales = "";

    // El primer evento que tiene que ocurrir es "A_recibeMensaje()"
    // por lo que su tiempo se pone en 0 y los demás en "infinito".
    m_A_recibeMensaje = 0.0;
    m_A_seLibera = INFINITY;
    m_B_recibeFrame = INFINITY;
    m_B_seLibera = INFINITY;

    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);

    connect(this, SIGNAL(avance(int)), ui->progressBar, SLOT(setValue(int)));
    connect(ui->actionAbout_simulador, SIGNAL(triggered(bool)), this, SLOT(about()));

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
            //llega aca significa que los campos necesarios estaban llenos
            m_modoLento = ui->checkBox->isChecked();
            ui->checkBox->setEnabled(false);
            ui->lineMaxTime->setEnabled(false);
            ui->lineNumVeces->setEnabled(false);
            ui->btnStart->setEnabled(false);
            ui->progressBar->setVisible(true);
            ui->resultadosFinales->setText("Los datos necesarios estan llenos, vamos a empezar la simulación.");
            correSimulacion();
        }else{
            ui->resultadosFinales->setText("No se asignó un máximo de tiempo.");
        }
    }else{
        ui->resultadosFinales->setText("No se asigno el número de veces a correr.");
    }
}

void Paso_Mensajes::about()
{
    vAbout = new About(this);
    vAbout->show();
}

void Paso_Mensajes::correSimulacion()
{
    double promPermanencia = 0; //media
    for(int i=0; i<m_numVeces; ++i){
        m_datosCPUA += "----------- Corrida número "+QString::number(i+1)+"-----------\n";
        while(m_reloj < m_maxTime){
            sigEvento();
            if(m_modoLento){
                QThread::msleep(10);      // hace una pausa de 1 segundo entre cada evento
            }
        }
        qDebug()<<"----- Termine la corrida "<<QString::number(i);

        double promColaA = 0;
        for(int i=0; i<tamColaA.size(); ++i){
            promColaA += tamColaA[i];
        }
        promColaA /= tamColaA.size();
        promTotalColaA.push_back(promColaA);
        m_datosCPUA += "- El tamaño promedio de la cola de A fue de: "+QString::number(promColaA, 'g', 3)+" mensajes\n";


        for(int i=0; i<permanencia.size(); ++i){
            promPermanencia += permanencia[i];
        }
        promPermanencia /= permanencia.size();
        promTotalPermanencia.push_back(promPermanencia);
        m_datosCPUA += "- El tiempo promedio de permanencia de un mensaje en el sistema fue de: "+ QString::number(promPermanencia, 'g', 3)+" segundos\n";

        promPermanencia = 0;

        m_datosCPUA += '\n';
        clear();
        ui->resultA->setText(m_datosCPUA);

        int progreso = ((i+1)*100)/m_numVeces;
        emit avance(progreso);
        QApplication::processEvents();
        QThread::msleep(300);
    }
    double promColaA = 0;
    for(int i=0; i<promTotalColaA.size(); ++i){
        promColaA += promTotalColaA[i];
    }
    promColaA /= promTotalColaA.size();
    datosGenerales += "- El promedio del tamaño de la cola de A en total: "+QString::number(promColaA, 'g', 3)+'\n';

    promPermanencia = 0;
    for(int i=0; i<promTotalPermanencia.size(); ++i){
        promPermanencia += promTotalPermanencia[i]; //individuales Yi
    }
    promPermanencia /= promTotalPermanencia.size();
    datosGenerales += "- El promedio de permanencia de un mensaje en el sistema en las "+QString::number(m_numVeces)+" corridas fue de:\n";
    datosGenerales += "     * "+QString::number(promPermanencia, 'g', 3)+" segundos.\n";

    if(m_numVeces == 10){
        /*
         * Se calcula el intervalo de confianza con la t-student
         *
         */
        int gradosLibertad = m_numVeces-1;// numVeces-1
        int sumatoria = 0;
        int sumatoriaParcial = 0;
        int varianza = 0;
        int intDeConfianza1 = 0;
        int intDeConfianza2 = 0;
        QString intDeConfianza = "";

        for (int i=0; i<9; i++){

            sumatoriaParcial = pow((promTotalPermanencia[i] - promPermanencia) , 2);
            sumatoria = sumatoriaParcial + sumatoria;
        }

        varianza = sumatoria/gradosLibertad;

        intDeConfianza1 = promPermanencia - 2.26 * pow ((varianza/10),(0,5));

        intDeConfianza2 = promPermanencia + 2.26 * pow ((varianza/10),(0,5));

        intDeConfianza = QString::number(intDeConfianza1) + " , " + QString::number(intDeConfianza2);
        datosGenerales += "- El intervalo de confianza es ["+intDeConfianza+"]\n";
    }

    if(m_numVeces >= 50){
        /*
         * Se calcula el intervalo de confianza con la distribucion normal
         */
        int gradosLibertad = m_numVeces-1;// numVeces-1
        int sumatoria = 0;
        int sumatoriaParcial = 0;
        int varianza = 0;
        int intDeConfianza1 = 0;
        int intDeConfianza2 = 0;
        QString intDeConfianza = "";

        for (int i=0; i<9; i++){

            sumatoriaParcial = pow((promTotalPermanencia[i] - promPermanencia) , 2);
            sumatoria = sumatoriaParcial + sumatoria;
        }

        varianza = sumatoria/gradosLibertad;

        intDeConfianza1 = promPermanencia - 1.96 * pow ((varianza/10),(0,5));

        intDeConfianza2 = promPermanencia + 1.96 * pow ((varianza/10),(0,5));

        intDeConfianza = QString::number(intDeConfianza1) + " , " + QString::number(intDeConfianza2);
        datosGenerales += "- El intervalo de confianza es ["+intDeConfianza+"]\n";
    }
    ui->resultadosFinales->setText(datosGenerales);
}

void Paso_Mensajes::A_recibeMensaje()
{

    m_reloj = m_A_recibeMensaje;    //R=LMA
    updateClock();

    mensaje newMensaje;
    newMensaje.numSecuencia = numMensajes;
    newMensaje.tiempoArrivo = m_reloj;
    newMensaje.seEnvio = false;
    qDebug()<<"Llego un mensaje a A";
    colaA.push_back(newMensaje);
    if(A_Ocupado == false){
        A_Ocupado = true;
        QThread::usleep(1);
        srand(time(NULL));
        double num = (rand()%100)*0.01;
        double varAleatoria = -(1/0.5)*(log(1-num)); //crea la variable aleatoria con dist. exponencial, parametro 1/2
        mensaje msjTmp = colaA[0];
        colaA[0] = msjTmp;
        m_A_seLibera = m_reloj + varAleatoria + 1;
    }
    ++ numMensajes;
    tamColaA.push_back(colaA.size());
    bool funciona = false;
    double varAleatoria;
    while(funciona == false){
        QThread::usleep(1);
        srand(time(NULL));
        double r1 = (rand()%100)*0.01;
        QThread::usleep(1);
        srand(time(NULL));
        double r2 = (rand()%100)*0.01;
        varAleatoria = 2 + pow(0.001, 1/2)*(pow((-2*log(r1)), 1/2)*sin(2*3.14*r2));    //tiempo de arribo con distribucion n(8, 0.001).
        if(varAleatoria > 0){
            funciona = true;
        }
    }
    m_A_recibeMensaje = m_reloj + varAleatoria;     //programa cuando va a arribar el siguiente mensaje
}

void Paso_Mensajes::A_seLibera()
{
    m_reloj = m_A_seLibera; //R = SLA
    updateClock();
    qDebug()<<"Entro a A_seLibera()";
    int indexMensaje = getMsjFaltante();
    if(indexMensaje != -1){
        frame newFrame;
        mensaje msjTemp = colaA[indexMensaje];
        newFrame.numSecuencia = msjTemp.numSecuencia;
        msjTemp.seEnvio = true;
        colaA[indexMensaje] = msjTemp;
        /*
         Ya no hay mensajes con error y tampoco se pierden de camino.
        */
        m_B_recibeFrame = m_reloj + 1;  //BRF = R+1
        colaB.push_back(newFrame);   //"lo pone en linea"

        int quedaAlguien = getMsjFaltante();
        if(quedaAlguien != -1){
            srand(time(NULL));
            int r = (rand()%100)*0.01;        //numero entre 0-1
            double varAleatoria = (-1/0.5)*log(1-r);  //crea la variable aleatoria con dist. exponencial, parametro 1/2
            m_A_seLibera = m_reloj + varAleatoria + 1;    //programa el evento Liberar A
        }else{
            A_Ocupado = false;
            m_A_seLibera = INFINITY;
        }
    }
}

void Paso_Mensajes::B_recibeFrame()
{
    m_reloj = m_B_recibeFrame;
    updateClock();
    qDebug()<<"Ocurre B_recibeFrame() en el tiempo: "<<QString::number(m_reloj,  'g', 5);
    if(!B_Ocupado && !colaB.empty()){ //el proceso esta libre -- ademas preguntar si la cola de B esta vacia
        B_Ocupado = true;
        srand (time(NULL));
        double aleat = ((rand() % 100)*0.01)+2;  //numero 2<=x<=3
        double varAleatoria = sqrt(5*aleat-4);
        m_B_seLibera = m_reloj + varAleatoria + 0.25;
    }
    m_B_recibeFrame = INFINITY;
}

void Paso_Mensajes::B_seLibera()
{
    m_reloj = m_B_seLibera;
    updateClock();

    mensaje msjTemp = colaA[0];
    permanencia.push_back(m_reloj - msjTemp.tiempoArrivo);
    colaB.erase(colaB.begin());
    colaA.erase(colaA.begin());
    //hay mas frames en colaB?
    if(!colaB.empty()){
        srand (time(NULL));
        double aleat = rand() % 2 + 2;  //numero 2<=x<=3
        double varAleatoria = (pow(aleat, 2.0)-4)/5;
        m_B_seLibera = m_reloj + varAleatoria + 0.25;
    }else{
        B_Ocupado = false;
        m_B_seLibera = INFINITY;
    }
}

void Paso_Mensajes::sigEvento()
{
    double vecTimes[] ={m_A_recibeMensaje, m_A_seLibera, m_B_seLibera, m_B_recibeFrame};
    std::list<double> lista;
    for(int i=0; i<4; ++i){
        lista.push_back(vecTimes[i]);
    }
    lista.sort();
    double tiempMin = lista.front();
    if(tiempMin == m_A_recibeMensaje){
        qDebug()<<"Evento que se esta ajecutando: A recibe mensaje.";
        A_recibeMensaje();
    }else{

        if(tiempMin == m_A_seLibera){
            qDebug()<<"Evento que se esta ajecutando: A se libera.";
            A_seLibera();
        }else{
            if(tiempMin == m_B_seLibera){
                qDebug()<<"Evento que se esta ajecutando: B se libera.";
                B_seLibera();
            }else{
                if(tiempMin == m_B_recibeFrame){
                    qDebug()<<"Evento que se esta ajecutando: B recibe frame.";
                    B_recibeFrame();
                }
            }
        }
    }
}

void Paso_Mensajes::clear()
{
    m_reloj = 0.0;
    colaA.clear();
    colaB.clear();
    tamColaA.clear();
    A_Ocupado = false;
    B_Ocupado = false;
    numMensajes = -1;
    m_A_recibeMensaje = 0;
    m_A_seLibera = INFINITY;
    m_B_recibeFrame = INFINITY;
    m_B_seLibera = INFINITY;

}

void Paso_Mensajes::updateClock()
{
    m_relojTotal += m_reloj;
    ui->labelReloj->setText(QString::number(m_reloj, 'f', 3)+ " segundos.");
    QApplication::processEvents();

}

int Paso_Mensajes::getMsjFaltante()
{
    int index = 0;
    while(index<colaA.size()){
        mensaje msjTemp = colaA[index];
        if(msjTemp.seEnvio == false){
            return index;
        }
        ++index;
    }
    return -1;
}
