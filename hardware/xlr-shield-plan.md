# Plan carte électronique — Shield Arduino Leonardo avec connecteurs XLR

## 1) Objectif
Carte au format **shield Arduino Leonardo** avec connecteurs XLR soudés sur PCB pour raccorder les faisceaux capteurs :
- F1 : Potentiomètre Axe X
- F2 : Potentiomètre Axe RX
- F3 : Encodeur Axe Y
- F4 : Encodeur Axe Z
- F5 : Bouton

Convention retenue (homogène sur tous les faisceaux) :
- **XLR pin 1 = GND (masse/blindage)**
- **XLR pin 2 = +5V (alimentation)**
- **pin 3+ = signaux** (1 signal pour potentiomètre/bouton, 2 signaux pour encodeur)

---

## 2) Schéma électrique (netlist fonctionnelle)

### Connecteurs
- J1 = XLR8_F1 (Axe X)
- J2 = XLR8_F2 (Axe RX)
- J3 = XLR8_F3 (Encodeur Y)
- J4 = XLR8_F4 (Encodeur Z)
- J5 = XLR8_F5 (Bouton)

### Liaisons Arduino Leonardo
- A0  -> J1 pin3
- A1  -> J2 pin3
- D2  -> J3 pin3
- D3  -> J3 pin4
- D0  -> J4 pin3
- D1  -> J4 pin4
- D4  -> J5 pin3
- +5V -> J1 pin2, J2 pin2, J3 pin2, J4 pin2, J5 pin2 (alimentation tous les XLR)
- GND -> J1 pin1, J2 pin1, J3 pin1, J4 pin1, J5 pin1 (masse/blindage tous les XLR)

### Pins non utilisées (XLR 8)
- J1: pin4..pin8 NC
- J2: pin4..pin8 NC
- J3: pin5..pin8 NC
- J4: pin5..pin8 NC
- J5: pin4..pin8 NC

### Polarisation des entrées
- **Aucune résistance pull-up externe** sur la carte shield.
- Les entrées concernées utilisent les **pull-up internes de l'Arduino Leonardo** (comme dans le firmware).

---

## 3) Protections / conditionnement (recommandé)

### Alimentation
- F1 (polyfuse réarmable) sur +5V entrant vers distribution capteurs : 500 mA

### Signaux
- Résistances série 220 Ω sur chaque ligne signal (A0, A1, D0, D1, D2, D3, D4)
- TVS ESD faible capa vers GND sur lignes exposées (option fortement recommandée)

---

## 4) BOM minimale
- 1x Empreinte shield Arduino Leonardo (headers empilables)
- 5x Connecteur XLR 8 broches femelle PCB/châssis compatible soudure
- 1x Polyfuse 500 mA resettable
- 7x Résistance 220 Ω (signaux)
- (Option) Réseau TVS ESD 7 voies ou TVS unitaire par ligne

---

## 5) Règles PCB (EasyEDA / Eagle)
- Carte 2 couches, cuivre 1 oz
- Largeur piste +5V/GND: >= 0.6 mm
- Largeur pistes signaux: >= 0.25 mm
- Plan de masse sur les 2 couches
- Connecteurs XLR alignés sur un bord de carte
- Trous de fixation mécaniques pour reprendre l’effort d’insertion/retrait des XLR
- Garder D0/D1 éloignés des pistes bruitées (UART partagé)

---

## 6) Nommage des nets (à reprendre tel quel)
- `GND_SENS` (pin 1 de tous les XLR)
- `+5V_SENS` (pin 2 de tous les XLR)
- `AXIS_X_SIG` (A0)
- `AXIS_RX_SIG` (A1)
- `ENC_Y_A` (D2)
- `ENC_Y_B` (D3)
- `ENC_Z_A` (D0)
- `ENC_Z_B` (D1)
- `BTN_0_SIG` (D4)

---

## 7) Notes d’intégration firmware
Le firmware actuel est compatible avec ce brochage :
- `controler/controler.ino`
- `controler/controler-with-calibration.ino`

Aucune modification de code n’est nécessaire si ce mapping est conservé.
