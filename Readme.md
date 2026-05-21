\# Simulateur Planeur ASK-21 - Interface Commandes de Vol



Ce projet présente le code source et le schéma de câblage pour digitaliser les commandes de vol d'un fuselage réel d'ASK-21. L'interface repose sur une carte \*\*Arduino Leonardo\*\*, configurée comme un périphérique de jeu USB natif (HID Joystick). 



\## ✈️ Présentation du Système



L'Arduino Leonardo émule un joystick USB avec 4 axes absolus et 1 bouton :

\* \*\*Axe X (Profondeur) :\*\* Géré par un potentiomètre 10k avec un embout jaune.

\* \*\*Axe RX (Gauchissement) :\*\* Géré par un potentiomètre 10k avec un embout orange.

\* \*\*Axes Y \& Z (Palonnier / Aérofreins / Trims) :\*\* Gérés par deux encodeurs rotatifs incrémentaux industriels (E38S6G5).

\* \*\*Bouton (ex: Radio ou Profil) :\*\* Un bouton poussoir câblé directement sur D4 avec la résistance de pull-up interne activée.



\---



\## 📋 Bill of Materials (BOM)



| Composant | Description | Quantité |

| :--- | :--- | :---: |

| \*\*Arduino Leonardo\*\* | Microcontrôleur avec support USB HID natif | 1 |

| \*\*Potentiomètre 10kΩ\*\* | Pour les axes analogiques | 2 |

| \*\*Encodeur E38S6G5\*\* | Encodeur rotatif incrémental (Plage standard configurée à 200 PPR) | 2 |

| \*\*Bouton Poussoir\*\* | Joystick bouton #0 | 1 |

| \*\*Condensateur 0.1 µF\*\* | \*(Optionnel)\* Filtrage matériel des potentiomètres | 2 |

| \*\*Condensateur 100 nF \& Résistance 100 Ω\*\* | \*(Optionnel)\* Anti-rebond matériel pour le bouton | 1 |



\---



\## 🔌 Câblage et Schémas (Wiring)



> ⚠️ \*\*Note importante pour l'Axe Z (Encodeur #2) :\*\* Les pins D0 et D1 de l'Arduino servent aussi à `Serial1`. Ne pas utiliser `Serial1` dans ce sketch sous peine de perturber la lecture de cet axe.



\### 1. Tableau Général des Connexions



| Organe / Axe | Capteur | Couleur Fil Capteur | Borne Arduino | Rôle / Signal |

| :--- | :--- | :--- | :---: | :--- |

| \*\*Axe X\*\* (Profondeur) | Potentiomètre 10k | Vert et Bleu<br>Blanc | \*\*5V et GND\*\*<br>\*\*A0\*\* | Alimentation / Masse<br>Curseur (Signal) |

| \*\*Axe RX\*\* (Gauchissement) | Potentiomètre 10k | Marron et Marron/Blanc<br>Vert | \*\*5V et GND\*\*<br>\*\*A1\*\* | Alimentation / Masse<br>Curseur (Signal) |

| \*\*Axe Y\*\* (Encodeur #1) | E38S6G5 | Rouge<br>Noir<br>Vert<br>Blanc | \*\*+5V\*\*<br>\*\*GND\*\*<br>\*\*D2\*\* (Voie A)<br>\*\*D3\*\* (Voie B) | Alimentation VCC<br>Masse / Blindage<br>Interruption<br>Interruption |

| \*\*Axe Z\*\* (Encodeur #2) | E38S6G5 | Rouge<br>Noir<br>Vert/Jaune<br>Blanc | \*\*+5V\*\*<br>\*\*GND\*\*<br>\*\*D0\*\* (Voie A)<br>\*\*D1\*\* (Voie B) | Alimentation VCC<br>Masse / Blindage<br>Signal<br>Signal |

| \*\*Bouton #0\*\* | Bouton Poussoir | N/A | \*\*D4\*\*<br>\*\*GND\*\* | Signal Bouton<br>Masse |



\### 2. Schémas de principe



\#### Câblage des Potentiomètres (Axes X \& RX)

```text

&#x20;     \[ +5V ] ─── (Extrémité - Vert ou Marron)

&#x20;                    │

&#x20;                   \[ ] Potentiomètre 10k

&#x20;                    │◀── (Curseur - Blanc ou Vert) ───\[ Pin A0 ou A1 ]

&#x20;                    │

&#x20;     \[  GND ] ─── (Extrémité - Bleu ou Marron/Blanc)

```



\#### Câblage du Bouton (Pin D4)

```text

&#x20;       5V (Interne Arduino)

&#x20;        │

&#x20;   \[ R\_pullup Interne ] (Activée par INPUT\_PULLUP)

&#x20;        │

&#x20;        ├─── Pin D4 ───\[ Bouton Poussoir ]─── GND

```

\*(Lecture à 1 (HIGH) au repos, 0 (LOW) quand appuyé)\*



\---



\## 🛠️ Configuration logicielle



Le code utilise les bibliothèques `Joystick.h` (par MHeironimus) et `Encoder.h` (par Paul Stoffregen).



\### Paramètres ajustables

Dans le fichier `.ino`, plusieurs paramètres peuvent être ajustés :

\* `POT\_DEADBAND` : Zone morte pour les potentiomètres (actuellement à `2`).

\* `PPR` : Impulsions par tour de l'encodeur (actuellement à `200`).

\* `TURNS\_FOR\_RANGE\_Y` \& `TURNS\_FOR\_RANGE\_Z` : Nombre de tours pour couvrir la plage de 0 à 1023 (actuellement à `1`).

\* `BTN\_DEBOUNCE\_MS` : Durée de l'anti-rebond logiciel en millisecondes (actuellement à `10`).

