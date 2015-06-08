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

    A_Ocupado = false;
    B_Ocupado = false;

    m_ultimoACKA = "";
    m_totFramesB = 0;
    m_eventoCorriendo = "";
    m_longColaA = 0;
    m_datosCPUB = "";
    m_recibidosCorrectosB = 0;

    numMensajes = 0;

    infinity = INFINITY;    // Se toma como infinito

    // El primer evento que tiene que ocurrir es "A_recibeMensaje()"
    // por lo que su tiempo se pone en 0 y los demás en "infinito".
    m_A_recibeMensaje = 0.0;
    m_A_seLibera = infinity;
    m_A_recibeACK = infinity;
    m_expiraTTL = infinity;
    m_B_recibeFrame = infinity;
    m_B_seLibera = infinity;

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
    int count = 0;
    while(m_reloj < m_maxTime && count < m_numVeces){
        sigEvento();
        ++count;
        if(m_modoLento){
            QThread::sleep(1);      // hace una pausa de 1 segundo entre cada evento
        }
    }
}

void Paso_Mensajes::A_recibeMensaje()
{
    mensaje newMensaje;
    m_reloj = m_A_recibeMensaje;    //R=LMA
    ++numMensajes;
    while(colaA.size()<8){
        newMensaje.numSecuencia = numMensaje;
        newMensaje.seEnvio = true;
        colaA.push(numMensajes);
        if(A_Ocupado == false){     //preparaFrame() esta libre
            A_Ocupado = true;
            srand(time(NULL));
            double num = rand()/RAND_MAX;   //numero entre 0-1
            double varAleatoria = -(1/0.5)*(log(1-num)); //crea la variable aleatoria con dist. exponencial, parametro 1/2
            mensajeActual = colaA.front();
            m_A_seLibera = m_reloj + varAleatoria + 1;    //programa el evento Liberar A
        }
    }
    if(colaA.size() >= 8){  //si ya no hay campo en la ventana
        colaA.push(numMensajes);
    }
    ui->resultA->setText("Mensajes recibidos: "+QString::number(colaA.size()));
    QString mensajeTmp = "Mensajes recibidos: "+QString::number(colaA.size());
    qDebug()<<mensajeTmp;

    srand(time(NULL));
    double r1 = rand()/RAND_MAX;
    srand(time(NULL));
    double r2 = rand()/RAND_MAX;
    double varAleatoria = 25 + pow((-2*log(r1)), 1/2)*sin(2*M_PI*r2);    //tiempo de arribo con distribucion n(25, 1).
    m_A_recibeMensaje = m_reloj + varAleatoria;     //programa cuando va a arribar el siguiente mensaje
    QString reloj = QString::number(m_reloj);
    ui->labelReloj->setText(reloj);  //indica en la interfaz cual es el reloj
    qDebug()<<"El reloj actual es: "+QString::number(m_reloj);
}

void Paso_Mensajes::A_seLibera()
{
    m_reloj = m_A_seLibera; //R = SLA
    qDebug()<<"El reloj actual es: "+QString::number(m_reloj);
    m_B_recibeFrame = m_reloj + 1;
    m_expiraTTL = m_reloj + m_timer;

    frame newFrame;
    newFrame.numSecuencia = mensajeActual;
    srand(time(NULL));
    if((rand()%100+1) <= 10){        //probabilidad 0.1 de que el mensaje vaya con error
        newFrame.error = true;
    }else{
        newFrame.error = false;
    }
    srand(time(NULL));
    if((rand()%100+1) <=5){      //probabilidad de 0.05 de que el mensaje se pierda

    }else{
        //programa el evento B_recibeFrame
        m_B_recibeFrame = m_reloj + 1;  //BRF = R+1
        colaB.push(newFrame);   //"lo pone en linea"
    }
    //luego hay que revisar si hay algo en cola
    if(!colaA.empty()){
        std::default_random_engine generador;
        std::exponential_distribution<double> distribucion(0.5);
        srand(time(NULL));
        int r = rand()/RAND_MAX;        //numero entre 0-1
        double varAleatoria = (-1/0.5)*log(1-r);  //crea la variable aleatoria con dist. exponencial, parametro 1/2
        m_A_seLibera = m_reloj + varAleatoria + 1;    //programa el evento Liberar A
    }else{
        A_Ocupado = false;
        m_A_seLibera = infinity;
    }

}

void Paso_Mensajes::A_recibeACK()
{

}

void Paso_Mensajes::expiraTTL()
{

}

void Paso_Mensajes::B_recibeFrame()
{
    m_reloj = m_B_recibeFrame;
    if(!B_Ocupado){ //el proceso esta libre
        B_Ocupado = true;
        srand (time(NULL));
        double aleat = rand() % 2 + 2;  //numero 2<=x<=3
        double varAleatoria = (pow(aleat, 2.0)-4)/5;
        m_B_seLibera = m_reloj + varAleatoria + 0.25;
    }
    /*
    else{
        deja el frame en la cola
    }
    */
}

void Paso_Mensajes::B_seLibera()
{
    m_reloj = m_B_seLibera;
    m_datosCPUB += "Ultimo ACK enviado por B --> ";
    if(colaB.front().numSecuencia == mensajeActual){    //es el que corresponde?
        if(colaB.front().error){    //viene con error?
            ACK = colaB.front().numSecuencia;
            m_datosCPUB += ACK+'\n';
            colaB.pop();
        }else{
            ACK = colaB.front().numSecuencia+1;
            ++m_recibidosCorrectosB;
            m_datosCPUB += ACK+'\n';
            ui->resultB->setText("Total de frames recibidos correctamente: "+QString::number(m_recibidosCorrectosB)+'\n');
            colaB.pop();
        }
    }else{
        ACK = mensajeActual;
        m_datosCPUB += ACK+'\n';
        colaB.pop();
    }
    ui->resultB->setText(m_datosCPUB);
    m_A_recibeACK = m_reloj + 1;
    //hay mas frames en colaB?
    if(!colaB.empty()){
        mensajeActual = ACK;
        srand (time(NULL));
        double aleat = rand() % 2 + 2;  //numero 2<=x<=3
        double varAleatoria = (pow(aleat, 2.0)-4)/5;
        m_B_seLibera = m_reloj + varAleatoria + 0.25;
    }else{
        B_Ocupado = false;
        m_B_seLibera = infinity;
    }
}

void Paso_Mensajes::sigEvento()
{
    double vecTimes[] ={m_A_recibeMensaje, m_A_seLibera, m_B_seLibera, m_B_recibeFrame, m_A_recibeACK, m_expiraTTL};
    // Busca el de menor tiempo
    double min = vecTimes[0];
    for(int i=0; i<6; ++i){
        for(int j=i+1; j<6; ++j){
            if(vecTimes[j]<min){
                min = vecTimes[j];
            }
        }
    }
    m_eventoCorriendo += "Evento que se esta procesando -> ";
    if(min == m_A_recibeMensaje){
        m_eventoCorriendo += "A recibe mensaje.\n";
        ui->resultadosFinales->setText(m_eventoCorriendo);
        A_recibeMensaje();
    }
    if(min == m_A_seLibera){
        m_eventoCorriendo += "A se libera.\n";
        ui->resultadosFinales->setText(m_eventoCorriendo);
        A_seLibera();
    }
    if(min == m_B_seLibera){
        m_eventoCorriendo += "B se libera.\n";
        ui->resultadosFinales->setText(m_eventoCorriendo);
        B_seLibera();
    }
    if(min == m_B_recibeFrame){
        m_eventoCorriendo += "B recibe frame.\n";
        ui->resultadosFinales->setText(m_eventoCorriendo);
        B_recibeFrame();
    }
    if(min == m_A_recibeACK){
        m_eventoCorriendo += "A recibe ACK.\n";
        ui->resultadosFinales->setText(m_eventoCorriendo);
        A_recibeACK();
    }
    if(min == m_expiraTTL){
        m_eventoCorriendo += "Expira TTL.\n";
        ui->resultadosFinales->setText(m_eventoCorriendo);
        expiraTTL();
    }

}
