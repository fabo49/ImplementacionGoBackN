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
    ACK = -1;

    totMensajesRecibidos = 0;

    A_Ocupado = false;
    B_Ocupado = false;

    m_ultimoACKA = "";
    m_totFramesB = 0;
    m_eventoCorriendo = "";
    m_longColaA = 0;
    m_datosCPUB = "";
    m_recibidosCorrectosB = 0;

    numMensajes = 1;

    // El primer evento que tiene que ocurrir es "A_recibeMensaje()"
    // por lo que su tiempo se pone en 0 y los demás en "infinito".
    m_A_recibeMensaje = 0.0;
    m_A_seLibera = INFINITY;
    m_A_recibeACK = INFINITY;
    m_expiraTTL = INFINITY;
    m_B_recibeFrame = INFINITY;
    m_B_seLibera = INFINITY;

    connect(this, SIGNAL(cambiaReloj(QString)),ui->labelReloj, SLOT(setText(QString)));

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
                ui->btnStart->setEnabled(false);
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

    m_reloj = m_A_recibeMensaje;    //R=LMA
    emit cambiaReloj(QString::number(m_reloj, 'g', 4));
    mensaje newMensaje;
    newMensaje.numSecuencia = numMensajes;
    newMensaje.seEnvio = false;
    qDebug()<<"Llego un mensaje a A";
    ++totMensajesRecibidos;
    m_expiraTTL = m_reloj + m_timer;
    colaTimer.push_back(m_expiraTTL);
    if(ventana.size()<8){    //Hay campo en la ventana?
        qDebug()<<"Se pone mensaje en la ventana.";
        newMensaje.venceTimer = m_expiraTTL;
        ventana.push_back(newMensaje);
        if(A_Ocupado == false){     //preparaFrame() esta libre?
            qDebug()<<"A_seLibera() esta libre, entonces lo programo";
            A_Ocupado = true;
            srand(time(NULL));
            double num = (rand()%100)*0.01;                 //numero entre 0-1
            double varAleatoria = -(1/0.5)*(log(1-num)); //crea la variable aleatoria con dist. exponencial, parametro 1/2
            mensaje msjTmp = ventana.front();
            mensajeActual = msjTmp.numSecuencia;
            m_A_seLibera = m_reloj + varAleatoria + 1;    //programa el evento Liberar A
        }
    }else{  //si ya no hay campo en la ventana
        qDebug()<<"Se pone mensaje en la cola de A.";
        colaA.push_back(newMensaje);
    }
    ++numMensajes;
    ui->resultA->setText("Mensajes recibidos: "+QString::number(totMensajesRecibidos, 10));
    QString mensajeTmp = "Mensajes recibidos: "+QString::number(totMensajesRecibidos, 10);
    qDebug()<<mensajeTmp;

    srand(time(NULL));
    double r1 = (rand()%100)*0.01;
    srand(time(NULL));
    double r2 = (rand()%100)*0.01;
    double varAleatoria = 25 + pow((-2*log(r1)), 1/2)*sin(2*M_PI*r2);    //tiempo de arribo con distribucion n(25, 1).
    m_A_recibeMensaje = m_reloj + varAleatoria;     //programa cuando va a arribar el siguiente mensaje
    QString reloj = QString::number(m_reloj, 'g', 5);
    ui->labelReloj->setText(reloj);  //indica en la interfaz cual es el reloj
    qDebug()<<"El reloj actual es: "<<QString::number(m_reloj, 'g', 5);
}

void Paso_Mensajes::A_seLibera()
{
    m_reloj = m_A_seLibera; //R = SLA
    qDebug()<<"Entro a A_seLibera()";
    int posMensaje = getMsjFaltante();
    if(posMensaje != -1){       //Si es -1 significa que todos los de la vantana se enviaron
        frame newFrame;
        mensaje msjTemp = ventana[posMensaje];
        msjTemp.seEnvio = true;
        ventana[posMensaje] = msjTemp;
        newFrame.numSecuencia = msjTemp.numSecuencia;

        srand(time(NULL));
        if((rand()%100+1) <= 10){        //probabilidad 0.1 de que el mensaje vaya con error
            newFrame.error = true;
            qDebug()<<"Se envia un frame con error";
        }else{
            newFrame.error = false;
            qDebug()<<"Se envia un frame sin error (No hay error)";
        }
        srand(time(NULL));
        if((rand()%100+1) <=5){      //probabilidad de 0.05 de que el mensaje se pierda
            qDebug()<<"Se va a perder el mensaje --> no se programa m_B_recibeFrame()";
        }else{
            //programa el evento B_recibeFrame
            m_B_recibeFrame = m_reloj + 1;  //BRF = R+1
            colaB.push_back(newFrame);   //"lo pone en linea"
        }

        int quedaAlguien = getMsjFaltante();
        if(quedaAlguien != -1){     //Queda alguien en la ventana?
            qDebug()<<"Estoy en A_seLibera() y todavia hay gente en la ventana";
            srand(time(NULL));
            int r = (rand()%100)*0.01;        //numero entre 0-1
            double varAleatoria = (-1/0.5)*log(1-r);  //crea la variable aleatoria con dist. exponencial, parametro 1/2
            msjTemp = ventana[quedaAlguien];
            mensajeActual = msjTemp.numSecuencia;
            //++mensajeActual;
            m_A_seLibera = m_reloj + varAleatoria + 1;    //programa el evento Liberar A
        }else{
            A_Ocupado = false;
            m_A_seLibera = INFINITY;
        }
    }
}

void Paso_Mensajes::A_recibeACK()
{
    m_reloj = m_A_recibeACK;
    qDebug()<<"Recibi un ACK que es: "<<QString::number(ACK, 10);
    mensaje mensajeTmp = ventana[0];
    int count = 0;
    while(mensajeTmp.numSecuencia < ACK && !ventana.empty()){
        ventana.erase(ventana.begin());
        colaTimer.erase(colaTimer.begin());
        if(!ventana.empty()){
            mensajeTmp = ventana[0];
        }
        if(!colaTimer.empty()){
            m_expiraTTL = colaTimer[0];
        }
        ++count;
        qDebug()<<"Va a expirar el timer en el segundo: "<<QString::number(m_expiraTTL, 'g', 5);
    }
    for(int i=0; i<count && !colaA.empty() && ventana.size() < 8; ++i){
        mensaje msjTemp = colaA[i];
        msjTemp.venceTimer = m_reloj + m_timer;
        colaTimer.push_back(m_reloj+m_timer);
        msjTemp.seEnvio = false;
        ventana.push_back(msjTemp);
        colaA.erase(colaA.begin());
    }
    m_A_recibeACK = INFINITY;
}

void Paso_Mensajes::expiraTTL()
{
    m_reloj = m_expiraTTL;
    qDebug()<<"Ocurrio expiraTTL() y estoy en el ciclo "<<QString::number(m_reloj,  'g', 5);
    if(!ventana.empty()){
        mensaje mensajeTmp = ventana.front();
        if(mensajeTmp.venceTimer == colaTimer.front()){     //Expira el timer del primer elemento
            colaTimer.clear();
            for(int i=0; i<ventana.size(); ++i){
                mensajeTmp = ventana[i];
                mensajeTmp.venceTimer = m_reloj + (i+1)*m_timer;
                mensajeTmp.seEnvio = false;
                ventana[i] = mensajeTmp;
                colaTimer[i] = mensajeTmp.venceTimer;
            }
            m_expiraTTL = colaTimer[0];
        }
    }else{
        m_expiraTTL = INFINITY;
    }
}

void Paso_Mensajes::B_recibeFrame()
{
    m_reloj = m_B_recibeFrame;
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
    m_datosCPUB += "Ultimo ACK enviado por B --> ";
    frame frame_tmp = colaB[0];
    if(frame_tmp.numSecuencia == mensajeActual){    //es el que corresponde?
        if(frame_tmp.error){    //viene con error?
            ACK = frame_tmp.numSecuencia;
            m_datosCPUB += QString::number(ACK, 10)+"\n";
            colaB.erase(colaB.begin());
        }else{
            ACK = frame_tmp.numSecuencia+1;
            ++m_recibidosCorrectosB;
            m_datosCPUB += QString::number(ACK, 10)+"\n";
            ui->resultB->setText("Total de frames recibidos correctamente: "+QString::number(m_recibidosCorrectosB, 10)+'\n');
            colaB.erase(colaB.begin());
        }
    }else{
        ACK = mensajeActual;
        m_datosCPUB += QString::number(ACK, 10)+"\n";
        colaB.erase(colaB.begin());
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
        m_B_seLibera = INFINITY;
    }
}

int Paso_Mensajes::getMsjFaltante()
{
    int index = 0;
    while(index< ventana.size()){
        mensaje msj_temp = ventana[index];
        if(msj_temp.seEnvio == false){
            return index;
        }
        ++index;
    }
    return -1;
}

void Paso_Mensajes::sigEvento()
{
    double vecTimes[] ={m_A_recibeMensaje, m_A_seLibera, m_B_seLibera, m_B_recibeFrame, m_A_recibeACK, m_expiraTTL};
    std::list<double> lista;
    for(int i=0; i<6; ++i){
        lista.push_back(vecTimes[i]);
    }
    lista.sort();
    double tiempMin = lista.front();
    m_eventoCorriendo += "Evento que se esta procesando -> ";
    if(tiempMin == m_A_recibeMensaje){
        m_eventoCorriendo += "A recibe mensaje.\n";
        ui->resultadosFinales->setText(m_eventoCorriendo);
        A_recibeMensaje();
    }else{

        if(tiempMin == m_A_seLibera){
            m_eventoCorriendo += "A se libera.\n";
            ui->resultadosFinales->setText(m_eventoCorriendo);
            A_seLibera();
        }else{
            if(tiempMin == m_B_seLibera){
                m_eventoCorriendo += "B se libera.\n";
                ui->resultadosFinales->setText(m_eventoCorriendo);
                B_seLibera();
            }else{
                if(tiempMin == m_B_recibeFrame){
                    m_eventoCorriendo += "B recibe frame.\n";
                    ui->resultadosFinales->setText(m_eventoCorriendo);
                    B_recibeFrame();
                }else{
                    if(tiempMin == m_A_recibeACK){
                        m_eventoCorriendo += "A recibe ACK.\n";
                        ui->resultadosFinales->setText(m_eventoCorriendo);
                        A_recibeACK();
                    }else{
                        if(tiempMin == m_expiraTTL){
                            m_eventoCorriendo += "Expira TTL.\n";
                            ui->resultadosFinales->setText(m_eventoCorriendo);
                            expiraTTL();
                        }
                    }
                }
            }
        }
    }
}
