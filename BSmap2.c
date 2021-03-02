#include <gtk/gtk.h> // on le met là sinon il faut compiler le .h (???)
#include "BSmap2.h"

int main(int argc,char *argv[])
{
	GtkBuilder *builder;
	GtkWidget *window;

	if (mkfifo(fiforead,0777) != 0 || mkfifo(fifowrite,0777) != 0)
	{
		printf("imposible de créer le pipe \n");
		//return;
	}
	printf("pipe OK\n");

	pthread_create(&mess,NULL,&manageMessages,NULL);
	pthread_create(&inp,NULL,&inputCommand,NULL);

	gtk_init(&argc, &argv);
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder,"BSmap2.glade",NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder,"window_main"));
	gtk_builder_connect_signals(builder,NULL);
	
	// connection des widgets, on peut en faire une structure pour mettre tout ça dans le .h
	pComboCal = GTK_WIDGET(gtk_builder_get_object(builder,"pComboCal"));
	pEntryCal = GTK_WIDGET(gtk_builder_get_object(builder,"pEntryCal"));
	pLabelPosPos = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelPosPos"));
	pLabelPosSpd = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelPosSpd"));
	pDraw = GTK_WIDGET(gtk_builder_get_object(builder,"pDraw"));
	pLabelConv = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelConv"));
	pLabelBlock = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelBlock"));
	pLabelDetect = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelDetect"));
	pLabelAssTar = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelAssTar"));
	pLabelAssType = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelAssType"));
	pLabelMotSpd = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelMotSpd"));
	pLabelMotPow = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelMotPow"));
	pLabelSenEnc = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelSenEnc"));
	pLabelSenSonar = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelSenSonar"));
	//pLabelAssTar = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelAssTar"));
	//pLabelAssTar = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelAssTar"));
	//pLabelAssTar = GTK_WIDGET(gtk_builder_get_object(builder,"pLabelAssTar"));
	g_object_unref(builder); 
	gtk_widget_show(window);
	gtk_main(); 
	 
	return 0;
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

			sprintf(text,"x = %4d\ny = %4d\na = %4d",curPos.posX,curPos.posY,curPos.posAlpha);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabelPosPos),(const gchar *)text);

			sprintf(text,"spd F = %4d\nspd R = %4d",curPos.spdFor,curPos.spdRot);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabelPosSpd),(const gchar *)text);

			gtk_widget_queue_draw(pDraw);
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
			if (curAss.conv==0)
				gtk_widget_override_background_color(GTK_WIDGET(pLabelConv),GTK_STATE_FLAG_NORMAL, &c_gray);
			else
				gtk_widget_override_background_color(GTK_WIDGET(pLabelConv),GTK_STATE_FLAG_NORMAL, &c_red);
			
			if (curAss.block==0)
				gtk_widget_override_background_color(GTK_WIDGET(pLabelBlock),GTK_STATE_FLAG_NORMAL, &c_gray);
			else
				gtk_widget_override_background_color(GTK_WIDGET(pLabelBlock),GTK_STATE_FLAG_NORMAL, &c_red);
			sprintf(text,"x : %d\ny : %d\na : %d",curAss.tarX,curAss.tarY,curAss.tarAlpha);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabelAssTar),(const gchar *)text);
			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabelAssType),(const gchar *)TypeNumToChar(curAss.type));

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
			curRob.score = (char) (trame[8]);
#ifdef GTK_USE
//			sprintf(text,"score %d",curRob.score);
//			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_SCORE]),(const gchar *)text);
//			sprintf(text,"t= %.1f",(float)curRob.count/50.0);
//			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_TIMER]),(const gchar *)text);
//			sprintf(text,"state %d",curRob.state);
//			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_STATE]),(const gchar *)text);
//			sprintf(text,"color %d",curRob.color);
//			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_COLOR]),(const gchar *)text);
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
//			sprintf(text,"  MOTORS  \nLeft = %3d\nRight = %3d\n",curMot.spdLeft,curMot.spdRight);
//			gtk_label_set_text(GTK_LABEL((GtkWidget *)pLabel[HMI_LABEL_MOT_MOT]),(const gchar *)text);
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
			gtk_entry_set_text(GTK_ENTRY(pEntryCal),(const gchar *)calstr);
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

// Thread qui va gérer l'envoi de messages en fonction de l'état de pollTrame

void *manageMessages( void *d)
{
	while(1)
	{
		if (pollTrame[0])
			sendReceive((char *)"P");//position
		if (pollTrame[1])
			sendReceive((char *)"M");//motors
		if (pollTrame[2])
			sendReceive((char *)"R");//robot
		if (pollTrame[3])
			sendReceive((char *)"V");//asserv
		if (pollTrame[4])
			sendReceive((char *)"S");//sensors
			sleeps(0.5);//on patiente
		//printf("inputready = %d\n",inputReady);
		if (inputReady == 0) // on a écrit un truc
		{
			sendReceive(strInput);
			inputReady = 1; // on autorise à nouveau l'input
		}
	}
	return NULL;
}

// Thread qui va gérer l'entrée stdio pour envoyer des trames hors IHM

void *inputCommand( void*g)
{
	//string s1;
	//char *s1;
	// thread qui va lire l'entrée standard 
	while(1)
	{
		if (inputReady)
		{
		//	getline(std::cin, s1); // pour récupérer même les espaces
		//	memcpy(strInput,s1.c_str(),s1.size());
		//	strInput[s1.size()] = '\0';
		//	if (strInput[0]=='l' || strInput[0] == 'L') // demande de log on/off
		//		logg = !logg;
		//	else
		//		inputReady = 0; // on va attendre que la commande soit envoyée
				
		}
		sleeps(0.5);
	}

    return NULL;
}

char checkSum()
{
	int cs=0;
	for (int i=0;i<strWrite[1]+3;i++)
		cs+=(int)strWrite[i];
	return (char)cs;
}

char *TypeNumToChar(int typ)
{
	switch (typ)
	{
		case ASS_NUL :
		default :
			return "NUL";
		case ASS_POLAR :
			return "POL";
		case ASS_POLARREV :
			return "REV";
		case ASS_ROTATION :
			return "ROT";
		case ASS_MANUAL :
			return "MAN";
		case ASS_PIVOT :
			return "PIV";
		case ASS_CIRCLE :
			return "CIR";
		case ASS_BLOCK :
			return "BLK";
	}
}

void getEntry(GtkEntry *entry, gpointer data)
{
	char *txt;
	memset(strInput,0,50);
	txt = (char *)gtk_entry_get_text(GTK_ENTRY(entry));
	memcpy(strInput,txt,strlen(txt));
	printf("string envoyée %s\n",txt);
	inputReady = 0;
}

void setCalValue(GtkEntry *entry)
{
	sprintf(strInput,"CS %s %s",
		gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(pComboCal)),
		gtk_entry_get_text(GTK_ENTRY(entry)));
	printf("cal\n");
	inputReady = 0;
}

// callback de destruction de la fenêtre
void on_window_main_destroy()
{
	
	gtk_main_quit();
}

// callback qui gère les boutons de demande de poll des trames
void on_Button_toggled(GtkToggleButton *togg)
{
	gboolean state= gtk_toggle_button_get_active(togg);
	char label[3];
	int index;
	strncpy(label,gtk_button_get_label(GTK_BUTTON(togg)),3);
	if (strncmp(label,"Pos",3)==0)
		index = HMI_BUTTON_POSITION;
	else if (strncmp(label,"Mot",3)==0)
		index = HMI_BUTTON_MOTORS;
	else if (strncmp(label,"Ass",3)==0)
		index = HMI_BUTTON_ASSERV;
	else if (strncmp(label,"Sen",3)==0)
		index = HMI_BUTTON_SENSOR;
	else if (strncmp(label,"Rob",3)==0)
		index = HMI_BUTTON_ROBOT;
	else if (strncmp(label,"Act",3)==0)
		index = HMI_BUTTON_ACTION;
	if (state)
		pollTrame[index]=1;
	else
		pollTrame[index] = 0;
}

void on_pComboCal_changed(GtkWidget *combo)
{
	sprintf(strInput,"CG %s",gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo)));
	inputReady = 0; // pour lancer l'envoi
//	printf("cahnge %s\n",gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo)));
}

void on_pDraw_draw(GtkWidget *widget,cairo_t *cr,gpointer data)
{
	guint width, height;
	GdkRGBA color;
	GtkStyleContext *context;
	context = gtk_widget_get_style_context(widget);
	
	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);
	
	gtk_render_background (context,cr,0,0,width,height);
	
	cairo_arc (cr, width/3000.0*curPos.posX, height - height/2100.0*curPos.posY, 5,0,2*G_PI);
	
	gtk_style_context_get_color(context,
								gtk_style_context_get_state(context),
								&color);
	gdk_cairo_set_source_rgba(cr,&color);
	
	cairo_fill(cr);

}
