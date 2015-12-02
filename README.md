# teleinfo_base
Sources de récupération de la télé-information d'un compteur BLEU pour abonnement BASE sur un Arduino.

Mon compteur est un compteur "BLEU" électronique monophasé multitarif (CBEMM).
J'ai un abonnement BASE, ce qui fait que je n'ai pas les heures pleines et les heures creuses.

Ce petit programme qui tourne sur un Arduino UNO avec un shield Ethernet, récupère les données de la télé-information et les envoie sur un Raspberry où j'ai un Jeedom qui tourne.

Je mettrai, bientôt, les sources du plugin Jeedom pour lire les données de la télé-information sur Github.
J'attends de le peaufiner un peu et aussi d'avoir des idées de présentation des données.

## Fonctionnement

Vous avez un fichier 'variables.ino', qui comme son nom l'indique, contient les variables pour le programme.
 + arduinoMAC: l'adresse MAC du shield Ethernet
 + arduinoIP: l'adresse IP que vous souhaitez attribuer à l'Arduino (IP static)
 + api_key: votre clef d'API Jeedom
 + client: L'adresse IP du Jeedom Master

## Montage de la carte de récupération de la télé-information
Pour les informations concernant la réalisation de la carte électronique, je vous renvoie vers l'article qui m'a servi sur [domotique-info.fr](http://www.domotique-info.fr/2014/05/recuperer-teleinformation-arduino/).

Pour le raccordement, je vous invite à bien faire attention à pas vous tromper dans le branchement sur le compteur. Il faut se raccorder sur les bornes I1 & I2 en bas à droite du compteur électronique.

Les informations d'ERDF concernant la téléinformation client (TIC) est disponible sur leur site: [documentation](http://www.erdf.fr/medias/DTR_Racc_Comptage/ERDF-NOI-CPT_02E.pdf).

## Problèmes rencontrés
 + J'ai remarqué que lorsque l'Arduino est connecté à l'ordinateur, j'ai un identifiant ACDO différent de celui de d'habitude. Je ne sais pas encore pourquoi.
 + J'ai remarqué aussi que par moment, les données ne sont pas correctes. Malgré le fait qu'une vérification est faite sur le checksum retourné par le compteur sur chaque ligne.

## Licence
Ces sources sont libres et peuvent être partagées sans aucune garantie quelconque.
