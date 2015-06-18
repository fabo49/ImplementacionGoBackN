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
    m_relojTotal = 0.0;
    ACK = -1;

    totMensajesRecibidos = 0;

    A_Ocupado = false;
    B_Ocupado = false;

    m_totFramesB = 0;
    m_longColaA = 0;
    m_datosCPUB = "";
    m_recibidosCorrectosB = 0;

    numMensajes = 1;

    datosGenerales = "";

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
    for(int i=0; i<m_numVeces; ++i){
        m_datosCPUA += "------- Corrida numero "+QString::number(i+1)+"-------\n";
        m_datosCPUB += "------- Corrida numero "+QString::number(i+1)+"-------\n";
        while(m_reloj < m_maxTime){
            sigEvento();
            if(m_modoLento){
                QThread::msleep(80);      // hace una pausa de 1 segundo entre cada evento
            }
        }
        qDebug()<<"----- Termine la corrida "<<QString::number(i);
        m_relojTotal += m_reloj;
        m_datosCPUA += "- Total de mensajes recibidos: "+QString::number(totMensajesRecibidos, 10)+'\n';

        double promColaA = 0;
        for(int i=0; i<tamColaA.size(); ++i){
            promColaA += tamColaA[i];
        }
        promColaA /= tamColaA.size();
        promTotalColaA.push_back(promColaA);
        m_datosCPUA += "- El tamaño promedio de la cola de A fue de: "+QString::number(promColaA, 'g', 3)+" mensajes\n";

        double promPermanencia = 0;
        for(int i=0; i<permanencia.size(); ++i){
            promPermanencia += permanencia[i];
        }
        promPermanencia /= permanencia.size();
        promTotalPermanencia.push_back(promPermanencia);
        m_datosCPUA += "- El tiempo promedio de permanencia de un mensaje en el sistema fue de: "+ QString::number(promPermanencia, 'g', 3)+" segundos\n";

        double promTrans = 0;
        for(int i=0; i<promTransmicion.size(); ++i){
            promTrans += promTransmicion[i];
        }
        promTrans /= promTransmicion.size();
        promTotalTransmicion.push_back(promTrans);
        m_datosCPUA += "- El tiempo promedio de transmición de un mensaje fue de: "+QString::number(promTrans, 'g', 3)+" segundos.\n";

        double tempServicio = promPermanencia - promTrans;
        m_datosCPUA += "- El tiempo promedio de servicio de un mensaje fue de: "+QString::number(tempServicio, 'g', 3)+" segundos.\n";
        double eficiencia = promTrans / tempServicio;
        m_datosCPUA += "- La eficiencia de "+QString::number(eficiencia*100, 'g', 3)+"%\n";

        m_datosCPUB += "- Total de frames recibidos correctamente: "+QString::number(m_recibidosCorrectosB, 10)+'\n';
        m_datosCPUA += '\n';
        m_datosCPUB += '\n';
        clear();
        ui->resultA->setText(m_datosCPUA);
        ui->resultB->setText(m_datosCPUB);
    }
    double promColaA = 0;
    for(int i=0; i<promTotalColaA.size(); ++i){
        promColaA += promTotalColaA[i];
    }
    ui->labelReloj->setText(QString::number(m_relojTotal, 'g', 3));
    promColaA /= promTotalColaA.size();
    datosGenerales += "- El promedio del tamaño de la cola de A en total: "+QString::number(promColaA, 'g', 3)+'\n';

    double promPermanencia = 0;
    for(int i=0; i<promTotalPermanencia.size(); ++i){
        promPermanencia += promTotalPermanencia[i];
    }
    promPermanencia /= promTotalPermanencia.size();
    datosGenerales += "- El promedio de permanencia de un mensaje en el sistema en las "+QString::number(m_numVeces)+" corridas fue de:\n";
    datosGenerales += "     * "+QString::number(promPermanencia, 'g', 3)+" segundos.\n";

    double promTrans = 0;
    for(int i=0; i<promTotalTransmicion.size(); ++i){
        promTrans += promTotalTransmicion[i];
    }
    promTrans /= promTotalTransmicion.size();
    datosGenerales += "- El promedio de transmicion de un mensaje en el sistema en las "+QString::number(m_numVeces)+" corridas fue de:\n";
    datosGenerales += "     * "+QString::number(promTrans, 'g', 3)+" segundos.\n";

    double tempServicio = promPermanencia - promTrans;
    datosGenerales += "- El tiempo promedio de servicio de un mensaje durante las "+QString::number(m_numVeces)+" corridas fue de:\n";
    datosGenerales += "        * "+QString::number(tempServicio, 'g', 3)+" segundos.\n";
    double eficiencia = promTrans / tempServicio;
    datosGenerales += "- La eficiencia durante los "+QString::number(m_numVeces)+" ciclos fue de "+QString::number(eficiencia*100, 'g', 3)+"%\n";

    ui->resultadosFinales->setText(datosGenerales);
}

void Paso_Mensajes::A_recibeMensaje()
{

    m_reloj = m_A_recibeMensaje;    //R=LMA

    mensaje newMensaje;
    newMensaje.numSecuencia = numMensajes;
    newMensaje.seEnvio = false;
    newMensaje.tiempoArrivo = m_reloj;
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
            QThread::msleep(2);
            srand(time(NULL));
            double num = (rand()%100)*0.01;                 //numero entre 0-1
            double varAleatoria = -(1/0.5)*(log(1-num)); //crea la variable aleatoria con dist. exponencial, parametro 1/2
            mensaje msjTmp = ventana[0];
            msjTmp.tiempoTransmision = varAleatoria +2;
            promTransmicion.push_back(msjTmp.tiempoTransmision);
            ventana[0] = msjTmp;
            mensajeActual = msjTmp.numSecuencia;
            m_A_seLibera = m_reloj + varAleatoria + 1;    //programa el evento Liberar A
        }
    }else{  //si ya no hay campo en la ventana
        qDebug()<<"Se pone mensaje en la cola de A.";
        colaA.push_back(newMensaje);
    }
    ++numMensajes;
    tamColaA.push_back(ventana.size()+colaA.size());

    QThread::msleep(2);
    srand(time(NULL));
    double r1 = (rand()%100)*0.01;
    QThread::msleep(2);
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
    srand(time(NULL));
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
        QThread::msleep(2);
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
            QThread::msleep(2);
            srand(time(NULL));
            int r = (rand()%100)*0.01;        //numero entre 0-1
            double varAleatoria = (-1/0.5)*log(1-r);  //crea la variable aleatoria con dist. exponencial, parametro 1/2
            msjTemp = ventana[quedaAlguien];
            mensajeActual = msjTemp.numSecuencia;
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
        qDebug()<<"Primer elemento en la ventana: "<<QString::number(mensajeTmp.numSecuencia);
        qDebug()<<"Hay "<<QString::number(ventana.size(), 10)<<" mensajes en la ventana";
        permanencia.push_back(m_reloj - mensajeTmp.tiempoArrivo);
        ventana.erase(ventana.begin());
        qDebug()<<"Borre de la ventana";
        qDebug()<<"El tamaño de la cola de timers es: "<<QString::number(colaTimer.size(), 10);
        colaTimer.erase(colaTimer.begin());
        qDebug()<<"Borre de la cola de timers";
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
    tamColaA.push_back(ventana.size()+colaA.size());
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
                mensajeTmp.venceTimer = m_reloj + m_timer;
                mensajeTmp.seEnvio = false;
                ventana[i] = mensajeTmp;
                colaTimer.push_back(mensajeTmp.venceTimer);
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
    frame frame_tmp = colaB[0];
    if(frame_tmp.numSecuencia == mensajeActual){    //es el que corresponde?
        if(frame_tmp.error){    //viene con error?
            ACK = frame_tmp.numSecuencia;
            colaB.erase(colaB.begin());
        }else{
            ACK = frame_tmp.numSecuencia+1;
            ++m_recibidosCorrectosB;
            colaB.erase(colaB.begin());
        }
    }else{
        ACK = mensajeActual;
        colaB.erase(colaB.begin());
    }
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
                }else{
                    if(tiempMin == m_A_recibeACK){
                        qDebug()<<"Evento que se esta ajecutando: A recibe ACK.";
                        A_recibeACK();
                    }else{
                        if(tiempMin == m_expiraTTL){
                            qDebug()<<"Evento que se esta ajecutando: expira Timer";
                            expiraTTL();
                        }
                    }
                }
            }
        }
    }
}

void Paso_Mensajes::clear()
{
    totMensajesRecibidos = 0;
    m_totFramesB = 0;
    m_longColaA = 0;
    m_recibidosCorrectosB = 0;
    m_reloj = 0.0;
    colaA.clear();
    ventana.clear();
    colaB.clear();
    colaTimer.clear();
    tamColaA.clear();
    ACK = -1;
    A_Ocupado = false;
    B_Ocupado = false;
    numMensajes = -1;
    m_A_recibeMensaje = 0;
    m_A_recibeACK = INFINITY;
    m_A_seLibera = INFINITY;
    m_expiraTTL = INFINITY;
    m_B_recibeFrame = INFINITY;
    m_B_seLibera = INFINITY;

}

void Paso_Mensajes::on_btnAbout_clicked()
{
    vAbout = new About(this);
    vAbout->show();
}
