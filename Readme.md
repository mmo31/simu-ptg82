# Simulateur Planeur ASK-21 - Interface Commandes de Vol

Ce projet présente le code source et le schéma de câblage pour digitaliser les commandes de vol d'un fuselage réel d'ASK-21. L'interface repose sur une carte **Arduino Leonardo**, configurée comme un périphérique de jeu USB natif (HID Joystick).

## ✈️ Présentation du Système

L'Arduino Leonardo émule un joystick USB avec 4 axes absolus et 1 bouton :

* **Axe X (Profondeur) :** Géré par un potentiomètre 10k avec un embout jaune.
* **Axe RX (Gauchissement) :** Géré par un potentiomètre 10k avec un embout orange.
* **Axes Y & Z (Palonnier / Aérofreins / Trims) :** Gérés par deux encodeurs rotatifs incrémentaux industriels (E38S6G5).
* **Bouton (ex: Radio ou Profil) :** Un bouton poussoir câblé directement sur D4 avec la résistance de pull-up interne activée.

---

## 📋 Bill of Materials (BOM)

| Composant | Description | Quantité |
| :--- | :--- | :---: |
| **Arduino Leonardo** | Microcontrôleur avec support USB HID natif | 1 |
| **Potentiomètre 10kΩ** | Pour les axes analogiques | 2 |
| **Encodeur E38S6G5** | Encodeur rotatif incrémental (Plage standard configurée à 200 PPR) | 2 |
| **Bouton Poussoir** | Joystick bouton #0 | 1 |
| **Connecteur XLR 8 broches mâle (côté Arduino)** | Un connecteur par faisceau capteur | 5 |
| **Connecteur XLR 8 broches femelle (contrepartie faisceau/châssis)** | Réception des faisceaux côté interface | 5 |
| **Câble multiconducteur 8 voies blindé** | Un câble dédié par capteur | 5 |
| **Condensateur 0.1 µF** | *(Optionnel)* Filtrage matériel des potentiomètres | 2 |
| **Condensateur 100 nF & Résistance 100 Ω** | *(Optionnel)* Anti-rebond matériel pour le bouton | 1 |

---

## 🔌 Câblage et Schémas (Wiring)

> ⚠️ **Note importante pour l'Axe Z (Encodeur #2) :** Les pins D0 et D1 de l'Arduino servent aussi à `Serial1`. Ne pas utiliser `Serial1` dans ce sketch sous peine de perturber la lecture de cet axe.

### 1. Nomenclature des faisceaux capteurs (1 capteur = 1 câble)

| Faisceau | Capteur | Connecteur côté Arduino | Couleur connecteur XLR 8 | Remarque |
| :---: | :--- | :---: | :--- | :--- |
| **F1** | Potentiomètre Axe X | XLR 8 mâle | **Rouge** | Câble dédié Axe X |
| **F2** | Potentiomètre Axe RX | XLR 8 mâle | **Bleu** | Câble dédié Axe RX |
| **F3** | Encodeur Axe Y | XLR 8 mâle | **Vert** | Câble dédié Encodeur #1 |
| **F4** | Encodeur Axe Z | XLR 8 mâle | **Jaune** | Câble dédié Encodeur #2 |
| **F5** | Bouton #0 | XLR 8 mâle | **Violet** | Câble dédié bouton |

### 2. Tableau Général des Connexions

> **Convention homogène** : Chaque capteur = 1 câble XLR 5 brins. **XLR pin 1 = GND (masse)**, **XLR pin 2 = +5V (alimentation)**, **pin 3+ = signaux**.

| Organe / Axe | Capteur | Faisceau | Couleur connecteur XLR 5 | Fil dans le câble XLR (1 fil / ligne) | Pin XLR 5 | Borne Arduino | Rôle / Signal |
| :--- | :--- | :---: | :--- | :--- | :---: | :--- | :--- |
| **Axe X** (Profondeur) | Potentiomètre 10k | **F1** | **Rouge** | Blindage/Noir<br>Rouge<br>Blanc | Pin 1<br>Pin 2<br>Pin 3 | **GND**<br>**+5V**<br>**A0** | Masse<br>Alimentation<br>Curseur (Signal) |
| **Axe RX** (Gauchissement) | Potentiomètre 10k | **F2** | **Bleu** | Blindage/Noir<br>Rouge<br>Blanc | Pin 1<br>Pin 2<br>Pin 3 | **GND**<br>**+5V**<br>**A1** | Masse<br>Alimentation<br>Curseur (Signal) |
| **Axe Y** (Encodeur #1) | E38S6G5 | **F3** | **Vert** | Blindage/Noir<br>Rouge<br>Vert<br>Blanc | Pin 1<br>Pin 2<br>Pin 3<br>Pin 4 | **GND**<br>**+5V**<br>**D2** (Voie A)<br>**D3** (Voie B) | Masse / Blindage<br>Alimentation VCC<br>Signal A<br>Signal B |
| **Axe Z** (Encodeur #2) | E38S6G5 | **F4** | **Jaune** | Blindage/Noir<br>Rouge<br>Vert/Jaune<br>Blanc | Pin 1<br>Pin 2<br>Pin 3<br>Pin 4 | **GND**<br>**+5V**<br>**D0** (Voie A)<br>**D1** (Voie B) | Masse / Blindage<br>Alimentation VCC<br>Signal A<br>Signal B |
| **Bouton #0** | Bouton Poussoir | **F5** | **Violet** | Noir<br>Rouge<br>Blanc | Pin 1<br>Pin 2<br>Pin 3 | **GND**<br>**(optionnel)**<br>**D4** | Masse<br>**(non utilisé)**<br>Signal bouton |

### 3. Schémas de principe

#### Câblage des Potentiomètres (Axes X & RX)

```text
	[ +5V ] ─── (Extrémité - Vert ou Marron)
				│
			    [ ] Potentiomètre 10k
				│◀── (Curseur - Blanc ou Vert) ─── [ Pin A0 ou A1 ]
				│
	[  GND ] ─── (Extrémité - Bleu ou Marron/Blanc)
```

#### Câblage du Bouton (Pin D4)

```text
	  5V (Interne Arduino)
	   │
   [ R_pullup Interne ] (Activée par INPUT_PULLUP)
	   │
	   ├─── Pin D4 ─── [ Bouton Poussoir ] ─── GND
```

*(Lecture à 1 (HIGH) au repos, 0 (LOW) quand appuyé)*

#### Repérage des broches d'un connecteur XLR 5 pins (utilisé pour tous les câbles)

> ℹ️ Les schémas ci-dessous sont donnés en **vue de face (côté accouplement)**. Toujours vérifier le marquage gravé sur le connecteur et la datasheet fabricant avant sertissage/soudure.

**XLR 5 mâle (vue de face, pins visibles)**

```text
              Détrompeur
                  ↑

            (3) (1) (2)
               (4) (5)
```

**XLR 5 femelle (vue de face, alvéoles visibles)**

```text
              Détrompeur
                  ↑

            (2) (1) (3)
               (5) (4)
```

### Affectation simplifiée pour chaque câble

Pour chaque faisceau (F1, F2, F3, F4, F5), les broches utilisées sont :
- **Pin 1 = GND (Noir)** — Masse/blindage commun
- **Pin 2 = +5V (Rouge)** — Alimentation
- **Pin 3+ = Signaux** — Selon capteur (potentiomètre = 1 signal, encodeur = 2 signaux, bouton = 1 signal)

---

## 🛠️ Configuration logicielle

Le code utilise les bibliothèques `Joystick.h` (par MHeironimus) et `Encoder.h` (par Paul Stoffregen).

### Deux programmes disponibles

Le dossier contient **deux versions** du firmware Arduino :

1. **Version standard** : `controler/controler.ino`  
	- Fonctionnement direct des axes, sans procédure de calibration utilisateur.

2. **Version avec calibration intégrée** : `controler/controler-with-calibration.ino`  
	- Permet d'enregistrer les valeurs min/max des capteurs directement depuis le cockpit, **sans passer par des menus Windows**.

### Calibration intégrée (version `controler-with-calibration.ino`)

Cette procédure se fait uniquement avec le bouton physique (D4) :

1. **Allumer** le système et laisser le joystick être reconnu normalement.
2. **Maintenir le bouton appuyé 5 secondes** pour entrer en mode calibration.
3. Bouger **tous les axes** sur toute leur course :
	- Potentiomètre Axe X : butée min ↔ butée max
	- Potentiomètre Axe RX : butée min ↔ butée max
	- Encodeur Axe Y : parcourir l'amplitude souhaitée min ↔ max
	- Encodeur Axe Z : parcourir l'amplitude souhaitée min ↔ max
4. **Relâcher** le bouton.
5. Faire **un appui court** (appuyer puis relâcher une fois) pour valider.
6. Les min/max sont sauvegardés en mémoire et la carte revient en mode normal.

> ✅ La calibration est mémorisée dans l'Arduino (EEPROM) et reste active après redémarrage.

> 🔁 Pour recalibrer, refaire simplement la même procédure.

### Paramètres ajustables

Dans le fichier `.ino`, plusieurs paramètres peuvent être ajustés :

* `POT_DEADBAND` : Zone morte pour les potentiomètres (actuellement à `2`).
* `PPR` : Impulsions par tour de l'encodeur (actuellement à `200`).
* `TURNS_FOR_RANGE_Y` & `TURNS_FOR_RANGE_Z` : Nombre de tours pour couvrir la plage de 0 à 1023 (actuellement à `1`).
* `BTN_DEBOUNCE_MS` : Durée de l'anti-rebond logiciel en millisecondes (actuellement à `10`).

---

## 🧩 Plan de carte électronique (Shield XLR)

Un plan de réalisation (prêt à saisir dans EasyEDA ou Eagle) est disponible ici :

- [hardware/xlr-shield-plan.md](hardware/xlr-shield-plan.md)
- [hardware/xlr-shield-netlist.csv](hardware/xlr-shield-netlist.csv)

Le plan reprend le même mapping que le firmware actuel (`A0`, `A1`, `D0..D4`, `+5V`, `GND`).

