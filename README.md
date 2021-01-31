# BSmap
Outil de communication avec le robot Brickstory 

le protocole est pour l'instant basé sur un com par pipe nommé
/tmp/BSmapRob pour les messages de BSmap vers le robot
/tmp/RobBSmap pour les messages du robot vers BSmap

BSmap est maitre, il envoie la trame et attend une réponse du robot
2 types de trames :
- ordre (pour un déplacement, une action, un paramètre)
- requête (pour obtenir une trame d'informations)

## entrées de l'interface
D num cns // actionneur : num est le numéro de l'actionneur, cns un ordre spécifique
TP valx valy // Consigne polaire, valx, valY la cible
TI valx valy // Consigne polaire en marche arrière, valx, valY la cible
TR vala // consigne de rotation vers l'angle vala

P // demande de trame de position
R // demande de trame de Robot
V // demande de trame d'asservissement
A // demande de trame d'actionneur
S // demande de trame de capteurs
X // demande d'un ack simple, pour test

# le protocole des trames
## ID des trames
ID_INFO 0x01
ID_ORDER 0x02
ID_ACK 0x03
ID_POSITION 0x10
ID_ASSERV 0x11
ID_ROBOT 0x12

## Protocole en émission

Ver est la version du robot = (année-2020) + 16*(1 pour principal ou 2 pour secondaire)
CS est le checksum (utile avec un pipe?)
id est l'identifiant de la trame demandée

demande de trame (5 octets)
> ID_INFO 1 Ver id CS

ordre de déplacement polaire
> ID_ORDER Ver P pfX pFX pfY pFY 0 0 CS

ordre de déplacement rotation
> ID_ORDER Ver R 0 0 0 0 pfA pFA CS

ordre d'actionneur (spécifique à chaque actionneur, on positionne 2 octets de base)
> ID_ORDER Ver A nact val1 val2

## Protocole en réception
Sur une demande de trame, c'est la trame en question qui doit être renvoyée
nu est le nombre d'octets utiles (hors id,version et CS)

Position (14 octets)
> ID_POSITION 10 Ver pfX pFX pfY pFY pfA PFA pfVf pFVf pfVr pFVr CS 

Robot (8 octets)
> ID_ROBOT 4 Ver color score count state CS