# Plan de placement PCB — Dimensions et positionnement

## 1) Dimensions carte (recommandé)

- **Largeur** : 110 mm (même que Arduino Leonardo)
- **Hauteur** : 80 mm (suffisant pour XLR alignés sur un bord)
- **Épaisseur** : 1.6 mm (standard 2 couches)
- **Matériau** : FR4, cuivre 1 oz

---

## 2) Disposition mécanique

```
┌─────────────────────────────────────┐
│  Bord entrée USB (haut de Leonardo) │
│                                     │
│    [H1 Headers - 2x15]              │  Côté opposé aux XLR
│                                     │  (reprise de hauteur)
│                                     │
│                                     │
│                                     │
│ [F1] [R1..R7]  <- petits composants │
│                                     │
│                                     │
│  [J1] [J2] [J3] [J4] [J5]           │  Bord XLR (bas)
│  ═════════════════════════════════  │  Trous de fixation
└─────────────────────────────────────┘
```

---

## 3) Positionnement détaillé des XLR

| Connecteur | X (mm) | Y (mm) | Rotation | Remarques |
| :---: | :---: | :---: | :---: | :--- |
| J1 (F1) | 10 | 5 | 0° | Axe X (gauche) |
| J2 (F2) | 32 | 5 | 0° | Axe RX |
| J3 (F3) | 54 | 5 | 0° | Encodeur Y |
| J4 (F4) | 76 | 5 | 0° | Encodeur Z |
| J5 (F5) | 98 | 5 | 0° | Bouton (droite) |

> Les connecteurs XLR sont alignés sur le bord inférieur (Y=5 mm) avec espacement régulier ~22 mm.

---

## 4) Positionnement headers Arduino

| Désignation | X (mm) | Y (mm) | Rotation | Remarques |
| :---: | :---: | :---: | :---: | :--- |
| H1 (partie haut) | 3 | 65 | 0° | Côté gauche |
| H1 (partie bas) | 107 | 65 | 0° | Côté droit |

> Les headers sont positionnés sur le bord opposé aux XLR, pour reprendre la hauteur d'empilage.

---

## 5) Positionnement composants petits (résistances, polyfuse)

| Désignation | X (mm) | Y (mm) | Remarques |
| :---: | :---: | :---: | :--- |
| F1 (polyfuse) | 12 | 35 | Zone centrale (alimentation) |
| R1..R7 (résistances) | 25..45 | 30..50 | Arborescence vers les XLR |

> Grouper les résistances de signal près des XLR pour minimiser longueur de piste.

---

## 6) Trous de fixation / ancrages mécaniques

Ajouter **4 trous M3** (ou M2.5) pour éviter flexion lors insertion/retrait des XLR :

| Trou | X (mm) | Y (mm) |
| :---: | :---: | :---: |
| TR1 | 8 | 8 |
| TR2 | 102 | 8 |
| TR3 | 8 | 72 |
| TR4 | 102 | 72 |

> Diam. trou : 3.2 mm (pour vis M3), ou 2.1 mm (pour vis M2.5).

---

## 7) Routage recommandé

### Alimentation +5V
- Largeur **piste** : >= 0.6 mm
- Source : pin `IOREF` ou `5V` de H1 -> F1 -> distribution XLR-J1..J5 pin 1
- **Plan de masse** sur couche inférieure pour retour GND

### Signaux
- Largeur **piste** : >= 0.25 mm
- Chaque signal passe par sa résistance 220 Ω
- Route avec **retour GND adjacent** (ou minimaliser boucle)

### GND (masse)
- **Plan de masse** sur au moins une couche
- Vias abondantes vers autres couches
- Point d'entrée unique sur F1 et H1 GND

---

## 8) Clearance / Contraintes de fabrication

- **Clearance trace-trace** : >= 0.2 mm
- **Clearance trace-via** : >= 0.25 mm
- **Via minimaliste** : 0.3 mm diam., 0.2 mm anneau
- **Trous de soudage** : >= 0.4 mm diam. (pour main-d'œuvre)

> ⚠️ EasyEDA génère souvent des règles par défaut compatibles 2 couches / PCB simple. Vérifier DRC avant envoi.

---

## 9) Impressions / couches

**Couche TOP (F.Cu)** :
- Pistes signal, alimentation
- Sérigraphie (valeurs, symboles)

**Couche BOTTOM (B.Cu)** :
- Plan de masse principal
- Quelques pistes retour signal (si nécessaire pour optimiser)

**Couches optionnelles** :
- F.Silkscreen : libellés position / valeurs
- F.Paste : pour assemblage automatisé (optionnel)

---

## 10) Schéma d'assemblage recommandé

1. **Soudure des vias / PCB** (si reflow)
2. **Soudure headers Arduino H1** (pré-soudés ou main)
3. **Soudure polyfuse F1**
4. **Soudure résistances R1..R7** (reflow ou fer)
5. **Soudure connecteurs XLR** (prise de soin mécanique)
6. **Soudure TVS optionnelle D1..D7**
7. **Nettoyage flux** (si nécessaire)
8. **Test continuité** avant intégration Arduino

---

## Estimation coûts PCB + composants

| Poste | Coût estimé |
| :--- | :---: |
| Fabrication PCB (10 pcs) | 5–15 € |
| Composants LCSC (1 jeu) | 8–12 € |
| **Total par unité** | **~2–3 € (composants)** |

> Délai : commande simultanée PCB + composants via JLCPCB/LCSC = 2–3 semaines.
