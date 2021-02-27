
#define GTK_USE

#ifdef GTK_USE
#include <cairo.h>
#include <gtk/gtk.h> // on le met là sinon il faut compiler le .h (???)
#include <glib.h>
#include <glib/gstdio.h>
#endif

#include "BSmap.h"

bool blabla = true;
//const char *list_command = "PVMRS";
//const char *list_command = "";//PVMRS";


const char * fiforead = "/tmp/RobBSmap"; // du robot vers l'outil
const char * fifowrite = "/tmp/BSmapRob"; // de l'outil vers le robot
int fd;

int main(int argc, char **argv)
{
#ifdef GTK_USE
	gtk_init(&argc, &argv);
#endif
	//char commande[50];
	// création des fifo, au cas où elles n'existeraient pas
	if (mkfifo(fiforead,0777) != 0 || mkfifo(fifowrite,0777) != 0)
	{
		printf("imposible de créer le pipe \n");
		//return;
	}
	printf("pipe OK\n");
	
	pthread_create(&inp,NULL,&inputCommand,NULL);
	pthread_create(&mess,NULL,&manageMessages,NULL);
	
	// pour l'instant on est en ligne de commande. On demande donc
	// une trame ou on donne un ordre par l'entrée strandard
	createHMI();
#ifdef GTK_USE
	gtk_main();
#else
	while (1) ;
#endif
	return(0);

}
int sendReceive(char *commande)
{
	char input;
	// encodage de la ligne reçue
	if (encode(commande)!=0)
		return -1;
	// puis écriture dans le pipe
	fd=open(fifowrite,O_WRONLY);
	if (fd != -1)
	{
		write(fd,strWrite, sizeWrite);
	}
	else
		printf("pipe HS\n");
	close (fd);
	//printf("Ecriture faite\n");
	
	//on va lire la réponse dans l'autre pipe
	fd=open(fiforead,O_RDONLY);
	if (fd != -1) // la lecture s'est bien passée
	{
		//printf("Reçu par le lecteur :\n");
		// traitement
		sizeRead=0;
		while(read(fd,&input,1)>0)
		{
			strRead[sizeRead++]=input;
			//printf("%c",input);
		}
		decode(strRead,sizeRead);
		//printf("La lecture se termine!\n");
	}
	else
		printf("pipe HS\n"); // oups!
	close (fd);
	//usleep(500000);
	//sleeps(0.1);
	return 0;
}

// types de commande, pour l'instant
// p ou P : demande de trame position
// r ou R : demande de trame robot
// v ou V : demande de trame asservissement
// a ou A : demande de trame actionneurs
// s ou s : demande de trame sensors
// t ou T : consigne de cible (target), TP pour polaire, TR pour rotation
// d ou D : consigne d'actionneur
// x ou X : test, on attend un ack simple
int encode(char *request)
{
	char tmpchar[3];
	int val1, val2,j;
	short vals1, vals2;
	
	if (strlen(request)==0)
		return -2;
	memset(strWrite,0,100);
	if (blabla){
	printf("Encodage de : %s\n",request);
	}
	switch (request[0]) // reçu depuis l'interface
	{
		default :
			printf("C'est quoi ça??\n");
			return (-1);
		case 'h':
		case 'H':
			printf("P : demande trame Position\n");
			printf("V : demande trame Asservissement\n");
			printf("R : demande trame Robot\n"); 
			printf("A : demande trame Actionneur\n");
			printf("S : demande trame Sensors\n");
			printf("M : demande trame motors\n");
			printf("TP valx valy : cible avant\n");
			printf("TI valx valy : cible arrière\n");
			printf("TR angle : cible en rotation\n");
			printf("TM vitDroite vitGauche : mode manuel en vitesse\n");
			printf("CG calib : renvoie la valeur de la calibration\n");
			printf("CS calib valeur : impose la valeur à la calibration\n");
			printf("CW : écriture de la config dans le fichier \n");
			printf("L : lancement / arrêt du log\n");
			printf("O : activation / désactivation détection obstacle\n");
			printf("H : help\n");
			break;
		case 'p':
		case 'P': // demande de position
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_POSITION;
			strWrite[4] = checkSum();
			break;
		case 'v':
		case 'V': // demande d'asservissement
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_ASSERV;
			strWrite[4] = checkSum();
			break;
		case 'r':
		case 'R':
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_ROBOT;
			strWrite[4] = checkSum();
			break;
		case 'a':
		case 'A':
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_ACTION;
			strWrite[4] = checkSum();
			break;
		case 's':
		case 'S':
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_SENSORS;
			strWrite[4] = checkSum();
			break;
		case 'm':
		case 'M': // demande de motors
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_MOTORS;
			strWrite[4] = checkSum();
			break;
		case 'o':
		case 'O': // obstacle
			sizeWrite = 4;
			strWrite[0] = ID_OBSTACLE;
			strWrite[1] = 0;
			strWrite[2] = VersionRobot;
			strWrite[3] = checkSum();
			break;
		case 't':
		case 'T':
			strWrite[0] = ID_ORDERMOVE;
			strWrite[2] = VersionRobot;
			if (request[1] == 'P' || request[1] == 'p') // polar
			{
				//strcpy(request,"TP 36 32");
				sscanf(request,"%s %d %d",tmpchar, &val1, &val2);
				printf("demande de polar vers (%d : %d)\n",val1, val2);
				vals1 = (short)val1;vals2=(short)val2;
				strWrite[3] = 'P'; //polar
				strWrite[4] = (char) ((int)vals1 & 0x00FF);
				strWrite[5] = (char) (((int)vals1)>>8 & 0x00FF);
				strWrite[6] = (char) ((int)vals2 & 0x00FF);
				strWrite[7] = (char) (((int)vals2)>>8 & 0x00FF);
				strWrite[8] = 0; //angle
				strWrite[9] = 0; //angle
				strWrite[10] = checkSum();
				strWrite[1] = 7;
				sizeWrite = 11;
			}
			else if (request[1] == 'M' || request[1] == 'm') // manual speed
			{
				//strcpy(request,"TP 36 32");
				sscanf(request,"%s %d %d",tmpchar, &val1, &val2);
				printf("demande de manual (d= %d : g= %d)\n",val1, val2);
				vals1 = (short)val1;vals2=(short)val2;
				strWrite[3] = 'M'; //polar
				strWrite[4] = (char) ((int)vals1 & 0x00FF);
				strWrite[5] = (char) (((int)vals1)>>8 & 0x00FF);
				strWrite[6] = (char) ((int)vals2 & 0x00FF);
				strWrite[7] = (char) (((int)vals2)>>8 & 0x00FF);
				strWrite[8] = checkSum();
				strWrite[1] = 5;
				sizeWrite = 9;
			}
			else if (request[1] == 'R' || request[1] == 'r') // rotation
			{
				//strcpy(request,"TP 36 32");
				sscanf(request,"%s %d",tmpchar, &val1);
				printf("demande de rotation vers un angle de %d° \n",val1);
				vals1 = (short)val1;
				strWrite[3] = 'R'; //polar
				strWrite[4] = (char) ((int)vals1 & 0x00FF);
				strWrite[5] = (char) (((int)vals1)>>8 & 0x00FF);
				strWrite[6] = checkSum();
				strWrite[1] = 3;
				sizeWrite = 7;
			}
			else if (request[1] == 'S' || request[1] == 's') // stop
			{
				//strcpy(request,"TP 36 32");
				printf("demande d'arrêt \n");
				strWrite[3] = 'S'; //polar
				strWrite[4] = checkSum();
				strWrite[1] = 1;
				sizeWrite = 5;
			}
			else
			{
				printf("Demande de déplacement inconnue. Tapez H pour l'aide\n");
				return -1;
			}
			break;
		case 'c':
		case 'C': // demande de calibration
			//strcpy(calname,request);
			strWrite[0] = ID_PARAM;
			strWrite[1] = strlen(request);
			strWrite[2] = VersionRobot;
			for (j=0;j<(int)strlen(request);j++)
				strWrite[j+3] = toupper(request[j]);
			strWrite[j+4] = checkSum();
			sizeWrite = strWrite[1]+4;
			
			for (j=0;j<max(0,(int)(strlen(request)))-3;j++)
				calname[j]=toupper(request[j+3]);
			calname[j+1]='\0';
			//printf("1");
			break;
		// les autres sont à faire
	}
	return 0;
}
int decode(char *trame,int t)
{
	char calstr[20];
	if (blabla){
	printf("début du décodage de ");
	for (int j=0;j<sizeRead;j++)
		printf("%x ",trame[j]);
	printf("\n");
	}
	switch (trame[0]) //l'identifiant
	{
		case ID_POSITION :
			//printf("demande de position t= %d\n",t);
			if (t < 13) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curPos.posX = (signed short)(trame[3])+((int)(trame[4]) << 8);
			curPos.posY = (signed short)(trame[5])+((int)(trame[6]) << 8);
			curPos.posAlpha = (signed short)(trame[7])+((int)(trame[8]) << 8);
			curPos.spdFor = (signed short)(trame[9])+((int)(trame[10]) << 8);
			curPos.spdRot = (signed short)(trame[11])+((int)(trame[12]) << 8);
#ifdef GTK_USE
			sprintf(text,"POSITION\nx = %4d\ny = %4d\na = %4d",curPos.posX,curPos.posY,curPos.posAlpha);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_POS_POS]),(const gchar *)text);
			//sprintf(text,"y = %d",curPos.posY);
			//gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_POS_Y]),(const gchar *)text);
			//sprintf(text,"a = %d",curPos.posAlpha);
			//gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_POS_ALPHA]),(const gchar *)text);
#endif
			if (blabla){
			printf("la position est %d %d %d\n",curPos.posX, curPos.posY, curPos.posAlpha);
			printf("la vitesse est %d mm/s %d°/s\n",curPos.spdFor, curPos.spdRot);
			}
			break; 
		case ID_ASSERV :
			if (t < 13) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curAss.type = trame[3];
			curAss.tarX = (signed short)(trame[4])+((int)(trame[5]) << 8);
			curAss.tarY = (signed short)(trame[6])+((int)(trame[7]) << 8);
			curAss.tarAlpha = (signed short)(trame[8])+((int)(trame[9]) << 8);
			curAss.spdFor = (signed short)(trame[10])+((int)(trame[11]) << 8);
			curAss.spdRot = (signed short)(trame[12])+((int)(trame[13]) << 8);
			curAss.conv = trame[14];
			curAss.block = trame[15];
#ifdef GTK_USE
			//sprintf(text,"tarX = %d",curAss.tarX);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_ASS_TAR]),getTextAssTar());
			//sprintf(text,"tarY = %d",curAss.tarY);
			//gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_TAR_Y]),(const gchar *)text);
			//sprintf(text,"tarA = %d",curAss.tarAlpha);
			//gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_TAR_ALPHA]),(const gchar *)text);
			//sprintf(text,"type = %d",curAss.type);
			//gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_TAR_TYPE]),(const gchar *)text);
#endif
			if (blabla){
			printf("type : %d converge = %d\n",curAss.type,curAss.conv);
			printf("la cible est %d %d %d\n",curAss.tarX, curAss.tarY, curAss.tarAlpha);
			printf("les vitesses for %d et rot %d\n",curAss.spdFor, curAss.spdRot);
			}
			break;
		case ID_ROBOT :
			if (t < 8) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curRob.state = trame[3];
			curRob.color = trame[4];
			curRob.count = (int)(trame[5])+((int)(trame[6]) << 8); // en top 20ms
			curRob.type = trame[7];
			curRob.score = (uint8_t) (trame[8]);
#ifdef GTK_USE
			sprintf(text,"score %d",curRob.score);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_SCORE]),(const gchar *)text);
			sprintf(text,"t= %.1f",(float)curRob.count/50.0);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_TIMER]),(const gchar *)text);
			sprintf(text,"state %d",curRob.state);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_STATE]),(const gchar *)text);
			sprintf(text,"color %d",curRob.color);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_COLOR]),(const gchar *)text);
#endif
			if (blabla){
			printf("count %.2f :score = %d\n",(float)(curRob.count)/50.0,curRob.score);
			}
			break;
		case ID_ACTION :
			if (t < 5) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curAct.state1 = trame[3];
			curAct.state2 = trame[4];
			//printf("count : %d score = %d\n",curRob.count,curRob.score);
			break;
		case ID_MOTORS :
			if (t < 5) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curMot.spdLeft = (signed short)(trame[3])+((int)(trame[4]) << 8);
			curMot.spdRight = (signed short)(trame[5])+((int)(trame[6]) << 8);
			curMot.powerLeft = trame[7];
			curMot.powerRight = trame[8];
#ifdef GTK_USE
			sprintf(text,"  MOTORS  \nLeft = %3d\nRight = %3d\n",curMot.spdLeft,curMot.spdRight);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_MOT_MOT]),(const gchar *)text);
#endif
			if (blabla){
			printf("vitesses moteurs : D %d G %d\n",curMot.spdRight, curMot.spdLeft);
			}
			break;
		case ID_SENSORS :
			if (t < 3) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curSen.angleLeft = (signed short)(trame[3])+((int)(trame[4]) << 8);
			curSen.angleRight = (signed short)(trame[5])+((int)(trame[6]) << 8);
			curSen.sonFrLeft = (unsigned char)trame[7];
			curSen.sonFrRight = (unsigned char)trame[8];
			curSen.sonReLeft = (unsigned char)trame[9];
			curSen.sonReRight = (unsigned char)trame[10];
			
			if (blabla){
			printf("codeurs : D %d G %d\n",curSen.angleRight, curSen.angleLeft);
			printf("sonars %d %d %d %d\n",curSen.sonFrLeft, curSen.sonFrRight,curSen.sonReLeft, curSen.sonReRight);
			}
			break;
		case ID_CALGET :
			memset(calstr,0,20);
			for (int i=0;i<(int)strlen(trame)-5;i++)
				calstr[i]=trame[i+3];
			printf("%s = %s\n",calname,calstr);
#ifdef GTK_USE
			gtk_entry_set_text(GTK_ENTRY(pEntryCal),(const gchar *)calstr);
#endif
			break;
		case ID_ACK:
			printf("Ack reçu\n");
			break;
		case ID_NACK:
			printf("NACK %d\n",trame[4]);
			break;
		default :
			printf("trame inconnue %d\n",trame[0]);
			break;
	}
	return 0;
}

// calcule le checksum, à savoir la somme de tous les octets, casté en char
char checkSum()
{
	int cs=0;
	for (int i=0;i<strWrite[1]+3;i++)
		cs+=(int)strWrite[i];
	return (char)cs;
}

/***** threads  ******/
void *inputCommand( void*)
{
	string s1;
	// thread qui va lire l'entrée standard 
	while(1)
	{
		if (inputReady)
		{
			getline(std::cin, s1); // pour récupérer même les espaces
			memcpy(strInput,s1.c_str(),s1.size());
			strInput[s1.size()] = '\0';
			if (strInput[0]=='l' || strInput[0] == 'L') // demande de log on/off
				log = !log;
			else
				inputReady = false; // on va attendre que la commande soit envoyée
				
		}
		sleeps(0.5);
	}

    return NULL;
}

void *manageMessages( void*)
{
#ifndef GTK_USE
	pollTrame[HMI_BUTTON_POSITION]=true;
	pollTrame[HMI_BUTTON_MOTORS]=true;
	pollTrame[HMI_BUTTON_ROBOT]=true;
	//pollTrame[HMI_BUTTON_ASSERV]=true;
	//pollTrame[HMI_BUTTON_POSITION]=true;
#endif
	while(1)
	{
		if (pollTrame[HMI_BUTTON_POSITION])
			sendReceive((char *)"P");//position
		if (pollTrame[HMI_BUTTON_MOTORS])
			sendReceive((char *)"M");//motors
		if (pollTrame[HMI_BUTTON_ROBOT])
			sendReceive((char *)"R");//robot
		if (pollTrame[HMI_BUTTON_ASSERV])
			sendReceive((char *)"V");//asserv
			//sendReceive((char *)"S");//sensors
			sleeps(0.5);//on patiente
		
		if (!inputReady) // on a écrit un truc
		{
			sendReceive(strInput);
			inputReady = true; // on autorise à nouveau l'input
		}
	}
	return NULL;
}

// interface graphique
int createHMI(void)
{
#ifdef GTK_USE

	c_red.alpha =1;c_red.red = 1;c_red.green = 0;c_red.blue=0;
	c_blue.alpha =1;c_blue.red = 0;c_blue.green = 0;c_blue.blue=1;
	c_green.alpha =1;c_green.red = 0;c_green.green = 1;c_green.blue=0;

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder,"BSmapHMI.glade",NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder,"window_main"));
	gtk_builder_connect_signals(builder,NULL);
	
	g_object_unref(builder);

	// la fenêtre principale
	//GtkWidget *pQuitBtn;
	//gchar *TexteConverti;
	
	/*
	// paramètres de la fenêtre
	pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(pWindow),"BSmap by Brickstory");
	gtk_window_set_default_size(GTK_WINDOW(pWindow),700,700);
	g_signal_connect(G_OBJECT(pWindow),"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	// table
	pTable=gtk_table_new(18,10,FALSE); //homogène
	//gtk_widget_
	gtk_container_add(GTK_CONTAINER(pWindow),GTK_WIDGET(pTable));
	
	pButton[HMI_BUTTON_POSITION]=gtk_button_new_with_label("Pos");
	pButton[HMI_BUTTON_ASSERV]=gtk_button_new_with_label("Ass"); 
	pButton[HMI_BUTTON_ROBOT]=gtk_button_new_with_label("Rob");
	pButton[HMI_BUTTON_MOTORS]=gtk_button_new_with_label("Mot");
	for (int j=0;j<4;j++)
		gtk_widget_override_color(GTK_WIDGET(pButton[j]),GTK_STATE_FLAG_NORMAL, &c_blue);
	// label
	pLabel[HMI_LABEL_POS_POS]=gtk_label_new("POSITION\nx = \ny = \na = ");
	//pLabel[HMI_LABEL_POS_Y]=gtk_label_new("y= ");
	//pLabel[HMI_LABEL_POS_ALPHA]=gtk_label_new("a= ");
	pLabel[HMI_LABEL_ASS_TAR]=gtk_label_new(" TARGET  \nx = \ny = \na = \n");
	pLabel[HMI_LABEL_MOT_MOT]=gtk_label_new(" MOTORS  \nLeft = \nRight = \n");
	//pLabel[HMI_LABEL_TAR_ALPHA]=gtk_label_new("tar a= ");
	pLabel[HMI_LABEL_SCORE]=gtk_label_new("Score ");
	pLabel[HMI_LABEL_STATE]=gtk_label_new("State ");
	pLabel[HMI_LABEL_COLOR]=gtk_label_new("Color");
	pLabel[HMI_LABEL_TIMER]=gtk_label_new("t = ");
	//pLabel[HMI_LABEL_TAR_TYPE]=gtk_label_new("type =");
	
	pEntry = gtk_entry_new(); // pour rentrer une commande directe
	
	pComboCal = gtk_combo_box_text_new();
	for (int i=0;i<NB_CAL;i++)
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pComboCal),NULL,listCal[i]);
	pEntryCal = gtk_entry_new(); // l'input de la valeur de calib
	
	pDraw = gtk_drawing_area_new();
	gtk_widget_set_size_request(pDraw,300,210);
	// attachement des widgets dans la table
	gtk_table_attach(GTK_TABLE(pTable),pButton[HMI_BUTTON_POSITION],13,14,1,2,GTK_EXPAND,GTK_EXPAND,0,0);
	gtk_table_attach(GTK_TABLE(pTable),pButton[HMI_BUTTON_ASSERV],14,15,1,2,GTK_EXPAND,GTK_EXPAND,0,0);
	gtk_table_attach(GTK_TABLE(pTable),pButton[HMI_BUTTON_ROBOT],15,16,1,2,GTK_EXPAND,GTK_EXPAND,0,0);
	gtk_table_attach(GTK_TABLE(pTable),pButton[HMI_BUTTON_MOTORS],16,17,1,2,GTK_EXPAND,GTK_EXPAND,0,0);

	gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_SCORE],0,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_STATE],3,5,1,2);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_COLOR],6,8,1,2);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_TIMER],9,11,1,2);

	gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_POS_POS],13,15,2,3);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_MOT_MOT],13,15,3,4);

	gtk_table_attach(GTK_TABLE(pTable),pComboCal,13,15,4,5,GTK_EXPAND,GTK_EXPAND,0,0);
	gtk_table_attach(GTK_TABLE(pTable),pEntryCal,15,16,4,5,GTK_EXPAND,GTK_EXPAND,0,0);

	gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_ASS_TAR],16,18,2,3);
	//gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_TAR_Y],16,18,3,4);
	//gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_TAR_ALPHA],16,18,4,5);
	//gtk_table_attach_defaults(GTK_TABLE(pTable),pLabel[HMI_LABEL_TAR_TYPE],16,18,5,6);

	gtk_table_attach(GTK_TABLE(pTable),pEntry,0,3,10,11,GTK_EXPAND,GTK_EXPAND,0,0);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pDraw,0,10,5,10);
//	gtk_widget_override_background_color(GTK_WIDGET(pLabel),GTK_STATE_FLAG_NORMAL, &c_blue);
//	gtk_widget_override_color(GTK_WIDGET(pLabel),GTK_STATE_FLAG_NORMAL, &c_red);
	for (int i=0;i<4;i++){
		g_signal_connect(G_OBJECT(pButton[i]),"clicked",G_CALLBACK(change),NULL);}
	g_signal_connect(G_OBJECT(pEntry),"activate",G_CALLBACK(getEntry),NULL);
	g_signal_connect(G_OBJECT(pComboCal),"changed",G_CALLBACK(getCalValue),NULL);
	g_signal_connect(G_OBJECT(pEntryCal),"activate",G_CALLBACK(setCalValue),NULL);
//	g_signal_connect(G_OBJECT(pDraw),"expose_event",G_CALLBACK(redraw),NULL);
	g_signal_connect(G_OBJECT(pDraw),"realize",G_CALLBACK(redraw2),NULL);
	//redraw2(pDraw,NULL);*/
/*	//gtk_container_add(GTK_CONTAINER(pWindow),pLabel);

	pQuitBtn = gtk_button_new_with_label("Quitter");
	g_signal_connect(G_OBJECT(pQuitBtn),"clicked",G_CALLBACK(gtk_main_quit),NULL);
	gtk_container_add(GTK_CONTAINER(pWindow),pQuitBtn);
*/
	//gtk_widget_show_all(pWindow);
	gtk_widget_show(window);
#endif
	return 0;
}

#ifdef GTK_USE
void change(GtkWidget *pb,gpointer data)
{
	for (int i=0;i<NB_TRAMES;i++)
	{
		if (pb == pButton[i])
		{
			//printf("chg %d\n",i);
			pollTrame[i] = ! pollTrame[i];
		}
	}// on découpe en deux sinon ça marche pas... je sais pas pourquoi
	for (int i=0;i<NB_TRAMES;i++)
	{
		if (pollTrame[i])
			gtk_widget_override_color(GTK_WIDGET(pButton[i]),GTK_STATE_FLAG_NORMAL, &c_red);
		else
			gtk_widget_override_color(GTK_WIDGET(pButton[i]),GTK_STATE_FLAG_NORMAL, &c_blue);
			// rajouter le changement de couleur du bouton
	}
//		gtk_label_set_text(GTK_LABEL((GtkWidget *)data),(const gchar *)("salut"));
//	if (pb == pButton[1])
//		gtk_label_set_text(GTK_LABEL((GtkWidget *)data),(const gchar *)("coucou"));

}
void getEntry(GtkEntry *entry, gpointer data)
{
	char *txt;//[50];// = NULL;//(char *)malloc(50);
	//memset(txt,0,50);
	memset(strInput,0,50);
	txt = (char *)gtk_entry_get_text(GTK_ENTRY(entry));
	//memcpy(strInput,(char *)gtk_entry_get_text(GTK_ENTRY(entry)),strlen((char *)gtk_entry_get_text(GTK_ENTRY(entry))));
	memcpy(strInput,txt,strlen(txt));
	printf("string envoyée %s\n",txt);
	inputReady = false;
//	free(txt);
}
void setCalValue(GtkEntry *entry, gpointer data)
{
	//char *txt;//[50];// = NULL;//(char *)malloc(50);
	//memset(txt,0,50);
	sprintf(strInput,"CS %s %s",
		gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(pComboCal)),
		gtk_entry_get_text(GTK_ENTRY(entry)));
//	memset(strInput,0,50);
	//txt = (char *)gtk_entry_get_text(GTK_ENTRY(entry));
	//memcpy(strInput,(char *)gtk_entry_get_text(GTK_ENTRY(entry)),strlen((char *)gtk_entry_get_text(GTK_ENTRY(entry))));
	//memcpy(strInput,txt,strlen(txt));
	//printf("string envoyée %s\n",txt);
	inputReady = false;
//	free(txt);
}


void getCalValue(GtkEntry *comb, gpointer data)
{
	//printf("%s\n",gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comb)));
	sprintf(strInput,"CG %s",gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comb)));
	inputReady = false; // pour lancer l'envoi
}

gboolean redraw2(GtkWidget *widget,gpointer userdata)
{
	cairo_t *cr=NULL;
	cr=gdk_cairo_create(gtk_widget_get_window(widget));
	cairo_set_source_rgb(cr,.1,.0,.0);
	cairo_set_line_width(cr,2.0);
	cairo_move_to(cr,10.,10.);
	cairo_line_to(cr,100.,100.);
	cairo_stroke(cr);
	cairo_destroy(cr);
	return false;
}

const gchar *getTextAssTar()
{
	//char txt[50];
	sprintf(text," TARGET  \nx = %4d\ny = %4d\na = %4d\n",curAss.tarX,curAss.tarY, curAss.tarAlpha);
	switch(curAss.type)
	{
		case ASS_NUL :
		default :
			strcat(text,"   NUL");break;
		case ASS_POLAR :
			strcat(text,"  POL AV");break;
		case ASS_POLARREV :
			strcat(text,"  POL_AR");break;
		case ASS_ROTATION :
			strcat(text," ROTATION");break;
		case ASS_MANUAL :
			strcat(text,"ASS_MANUAL");break;
	}
	return (const gchar *)text;
}

void on_wondow_main_destroy()
{
	gtk_main_quit();
}

#endif
