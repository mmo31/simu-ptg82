# BOM détaillée avec références LCSC/EasyEDA

## Connecteurs principaux

| Pos | Désignation | Valeur | Quantité | LCSC Part # | Empreinte | Remarques |
| :---: | :--- | :--- | :---: | :--- | :--- | :--- |
| J1-J5 | Connecteur XLR 5 femelle | XLR 5F PCB | 5 | C409140 (ou similaire) | XLR5_FEMALE_PCB | Soudure directe PCB, corps plastic |
| H1 | Headers Arduino Leonardo | 30 broches x2 | 1 kit | C265909 (ou kit shield) | HEADER_2x15 | Empilables (stacking headers) |

## Protection alimentation

| Pos | Désignation | Valeur | Quantité | LCSC Part # | Empreinte | Remarques |
| :---: | :--- | :--- | :---: | :--- | :--- | :--- |
| F1 | Polyfuse réarmable | 500 mA / 6V | 1 | C405169 | 0805 | Protège la distribution +5V |

## Résistances signaux

| Pos | Désignation | Valeur | Quantité | LCSC Part # | Empreinte | Remarques |
| :---: | :--- | :--- | :---: | :--- | :--- | :--- |
| R1 | Résistance série | 220 Ω | 7 | C25741 (1/4W) | 0603 | A0, A1, D0, D1, D2, D3, D4 |

## TVS optionnelle (fortement recommandée)

| Pos | Désignation | Valeur | Quantité | LCSC Part # | Empreinte | Remarques |
| :---: | :--- | :--- | :---: | :--- | :--- | :--- |
| D1-D7 | Diode TVS ESD | 5V / bi-directionnelle | 7 (opt.) | C452048 (ou réseau) | SOD-323 | Protection ESD chaque signal |

---

## Notes d'approvisionnement

- **LCSC** : fournisseur principal (partenaire EasyEDA)
- **Alternatives** : Digi-Key, Mouser (mêmes références MPN)
- **Délai** : commande sur LCSC ~1-2 semaines (selon région)
- **MOQ** (Minimum Order Quantity) : généralement 5 pcs connecteurs XLR

---

## Montage estimé
- Soudure des connecteurs XLR (manuelle ou reflow) : **principal effort**
- Soudure des résistances (0603) : facile
- Soudure des headers Arduino : facile (peuvent être pré-soudés)
- Soudure de la polyfuse : facile

**Durée approximée** : 20-30 min (manuel), plaçage + reflow < 10 min (automatisé).
