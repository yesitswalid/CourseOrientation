# CourseOrientation

Application Project made for school with framework Qt version 6

L'application fonctionne en multi-plateforme tel que Linux et Windows.

Libraries necessaires pour la fonctionnalité de l'application les pilotes (drivers) QMySQL, QSQLite pour la gestion de données
Ainsi que le port de série QSerialPort.

![Ibb Image](https://i.ibb.co/mDr991G/IHM.png)


# La description en Francais

Le but du projet consiste à développer une solution pour fournir au club de course d'orientation une solution aux compétiteurs un moyen de se pré-inscrire à la compétition, 
ainsi que de s'inscrire le jour même sans avoir besoin de connexion internet. 

Les organisateurs auront à disposer des balises que les orienteurs devront atteindre pour valider leur passage aux différents points de contrôle.
Les balises étaient des perforeuses ayant existé depuis longtemps avec une particularité distincte, 
On n'a donc opté pour une technologie qui remplace cela par des lecteurs qui viendront inscrire les informations de passage sur le badge RFID. 

Le jour de la course les orienteurs devront passer par un portique RFID pour valider leur départ. 
Une fois que les participants ont terminé la course, ils devront se présenter vers le portique pour pouvoir décharger leur badge, 
Le portique enverra une trame vers le PC Organisateur (Application) à l'aide d'une communication client-serveur de Protocol UDP (User Data Protocol) pour pouvoir récupérer l'historique des heures de passages des balises et de l'heure arriver de l'orienteur, 
Ces informations seront ensuite stockées dans une base de données local de type SQLite du côté PC Organisateur. 

Lorsque la course terminer les données de la base de données local du côté du PC Organisateur sera traité et toutes les informations téléversée vers le site web qui possède une base de données de type MySQL. 

L'organisateur du site traitera ces données et devra établir un classement ainsi que les statistiques des participants.


# English description

The goal of the project is to develop a solution to provide orienteering clubs with a way to pre-register for the competition,
as well as to register on the day of the competition without the need for an internet connection.

The organizers will have to arrange the beacons that the orienteers will have to reach to validate their passage to the different checkpoints.
The beacons were punches that existed for a long time with a distinct feature, 
so we opted for a technology that replaces this with readers that will write the passage information on the RFID badge.
On the day of the race, 
orienteers will have to pass through an RFID gate to validate their start. 

Once the participants have finished the race, they will have to go to the portal to unload their badge, 
the portal will send a frame to the PC Organizer (Application) using a client-server communication of UDP Protocol (User Data Protocol) to retrieve the history of the times of passage of the beacons and the time of arrival of the orienteer,
this information will then be stored in a local database type SQLite on the side of the PC Organizer. 
When the race finishes the data in the local database on the PC Organizer side will be processed and all the information uploaded to the website which has a MySQL type database. 

The organizer of the site will process this data and will have to establish a classification as well as the statistics of the participants.

Project work in cross platform not only Linux and windows include.
