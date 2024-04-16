// FranceEditor-1.00.cpp : définit le point d'entrée de l'application.
//

#include "FranceEditor-1.00.h"

using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <filesystem>

class Collectivite {
public:
    char type;
    std::string code;
    std::string nom;
    std::string appartientA;
    std::map<std::string, int> nombreCollectivites;
    bool affiche;

    Collectivite(char type, std::string code, std::string nom, std::string appartientA)
        : type(type), code(code), nom(nom), appartientA(appartientA), affiche(false) {}

    void ajouterTypeCollectivite(const std::string& typeCollectivite, int nombre = 0) {
        nombreCollectivites[typeCollectivite] = nombre;
    }

    void lireCSV(const std::string& nomFichier) {
        std::ifstream fichier(nomFichier);
        std::string ligne;
        if (std::getline(fichier, ligne)) {
            std::istringstream ss(ligne);
            std::string typeCollectivite;
            while (std::getline(ss, typeCollectivite, ',')) {
                if (!typeCollectivite.empty() && nombreCollectivites.find(typeCollectivite) == nombreCollectivites.end()) {
                    ajouterTypeCollectivite(typeCollectivite);
                }
            }
        }
        while (std::getline(fichier, ligne)) {
            std::istringstream ss(ligne);
            std::string valeur;
            auto it = nombreCollectivites.begin();
            while (std::getline(ss, valeur, ',') && it != nombreCollectivites.end()) {
                it->second = std::stoi(valeur);
                ++it;
            }
        }
    }

    void afficherDetails() const {
        std::cout << "Type de collectivité : " << type << std::endl;
        std::cout << "Code de la collectivité : " << code << std::endl;
        std::cout << "Nom de la collectivité : " << nom << std::endl;
        std::cout << "Appartient à : " << appartientA << std::endl;
        for (const auto& pair : nombreCollectivites) {
            std::cout << "Nombre de " << pair.first << " : " << pair.second << std::endl;
        }
        std::cout << std::endl;
    }
};

void afficherCollectivites(std::vector<Collectivite>& listeCollectivites, const std::string& param = "", char type = 'T') {
    for (auto& collectivite : listeCollectivites) {
        if (type == 'T' || collectivite.type == type) {
            if (param.empty() || collectivite.code == param) {
                if (!collectivite.affiche) {
                    collectivite.afficherDetails();
                    collectivite.affiche = true;
                }
                if (collectivite.type == 'R' && type == 'T') {
                    for (auto& departement : listeCollectivites) {
                        if (departement.type == 'D' && departement.appartientA == collectivite.code && !departement.affiche) {
                            departement.afficherDetails();
                            departement.affiche = true;
                        }
                    }
                }
            }
        }
    }
    for (auto& collectivite : listeCollectivites) {
        collectivite.affiche = false;
    }
}

void initialiserCollectivites(std::vector<Collectivite>& listeCollectivites, std::vector<std::string>enTete) {
    char choix;
    do {
        std::cout << "Voulez-vous créer un tableau manuellement (M) ou à partir d'un fichier CSV (C), ou quitter (Q) ? ";
        std::cin >> choix;
        if (choix == 'M' || choix == 'm' || choix == 'C' || choix == 'c' || choix == 'Q' || choix == 'q') {
            std::cout << "Attention : cela va reinitialiser le tableau entièrement. Etes-vous sur ? (O/N) ";
            char confirmer;
            std::cin >> confirmer;
            if (confirmer == 'O' || confirmer == 'o') {
                listeCollectivites.clear();
                if (choix == 'M' || choix == 'm') {
                    std::cout << "Combien de categories de collectivites voulez-vous ajouter ? ";
                    int nbCategorieColl;
                    std::cin >> nbCategorieColl;
                    std::vector<std::string> enTete = { "Type", "Code", "Nom", "Appartient a" };
                    for (int i = 0; i < nbCategorieColl; ++i) {
                        std::string nomColonne;
                        std::cout << "Entrez le nom de la colonne supplémentaire " << (i + 1) << " : ";
                        std::cin >> nomColonne;
                        enTete.push_back(nomColonne);
                    }
                    std::ofstream fichier("en-tete.csv");
                    for (size_t i = 0; i < enTete.size(); ++i) {
                        fichier << enTete[i];
                        if (i != enTete.size() - 1) {
                            fichier << ",";
                        }
                    }
                    fichier << "\n";
                    fichier.close();
                    std::cout << "L'en-tête du CSV a été créé avec succès.\n";
                }
                else if (choix == 'C' || choix == 'c') {
                    std::cout << "Entrez le nom du fichier CSV : ";
                    std::string nomFichier;
                    std::cin >> nomFichier;
                    nomFichier = nomFichier + ".csv";
                    std::ifstream fichier(nomFichier);
                    if (!fichier.is_open()) {
                        std::cout << "Impossible d'ouvrir le fichier CSV." << std::endl;
                        return;
                    }
                    std::string ligne;
                    std::getline(fichier, ligne);

                    std::istringstream ss(ligne);
                    std::vector<std::string> enTete;
                    std::string colonne;
                    while (std::getline(ss, colonne, ',')) {
                        enTete.push_back(colonne);
                    }
                    int nbColonnesSupp = enTete.size() - 4;

                    while (std::getline(fichier, ligne)) {
                        std::istringstream ss(ligne);
                        char type;
                        std::string code;
                        std::string nom;
                        std::string appartientA;
                        ss >> type;
                        ss.ignore();
                        std::getline(ss, code, ',');
                        std::getline(ss, nom, ',');
                        std::getline(ss, appartientA, ',');
                        Collectivite nouvelleCollectivite(type, code, nom, appartientA);
                        for (int i = 0; i < nbColonnesSupp; ++i) {
                            std::string nombreStr;
                            std::getline(ss, nombreStr, ',');
                            int nombre = std::stoi(nombreStr);
                            nouvelleCollectivite.ajouterTypeCollectivite(enTete[i + 4], nombre);
                        }
                        listeCollectivites.push_back(nouvelleCollectivite);
                    }
                    fichier.close();
                    return;
                }
                else {
                    std::cout << "Choix invalide. Veuillez entrer M pour une saisie manuelle ou C pour une saisie à partir d'un fichier CSV." << std::endl;
                }
            }
        }
    } while (choix != 'Q' && choix != 'q');
}


template <typename T>
void demanderEntree(const std::string& prompt, T& valeur) {
    std::cout << prompt;
    std::string entree;

    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }
    std::getline(std::cin, entree);
    if (!entree.empty()) {
        std::istringstream iss(entree);
        T nouvelleValeur;
        iss >> nouvelleValeur;
        if (!iss.fail()) {
            valeur = nouvelleValeur;
        }
    }
}

void recueillirInfosCollectivite(Collectivite& collectivite, const std::vector<std::string>& enTete) {
    demanderEntree("Entrez le type de la collectivité (R ou D) ou appuyez deux fois sur Entrée pour conserver la valeur actuelle (" + std::string(1, collectivite.type) + ") : ", collectivite.type);
    demanderEntree("Entrez le code de la collectivité (max 4 caractères) ou appuyez deux fois sur Entrée pour conserver la valeur actuelle (" + collectivite.code + ") : ", collectivite.code);
    demanderEntree("Entrez le nom de la collectivité ou appuyez deux fois sur Entrée pour conserver la valeur actuelle (" + collectivite.nom + ") : ", collectivite.nom);
    demanderEntree("Entrez le nom de la collectivité à laquelle elle appartient ou appuyez deux fois sur Entrée pour conserver la valeur actuelle (" + collectivite.appartientA + ") : ", collectivite.appartientA);
    for (auto& pair : collectivite.nombreCollectivites) {
        demanderEntree("Entrez le nombre de " + pair.first + " ou appuyez deux fois sur Entrée pour conserver la valeur actuelle (" + std::to_string(pair.second) + ") : ", pair.second);
    }
}

void mettreAJourRegions(std::vector<Collectivite>& listeCollectivites, std::vector<std::string>enTete) {
    for (auto& collectivite : listeCollectivites) {
        if (collectivite.type == 'R') { // Si la collectivité est une région
            int total = 0;
            for (const auto& autreCollectivite : listeCollectivites) {
                if (autreCollectivite.type == 'D' && autreCollectivite.appartientA == collectivite.code) { // Si c'est un département de la région
                    total += 1;
                }
            }
            collectivite.nombreCollectivites["Dep"] = total; // Mettre à jour le nombre de départements de la région
        }
    }
}

void creerCollectivite(std::vector<Collectivite>& listeCollectivites, const std::vector<std::string>& enTete) {
    char type;
    std::string code;
    std::string nom;
    std::string appartientA;
    Collectivite nouvelleCollectivite(type, code, nom, appartientA);
    recueillirInfosCollectivite(nouvelleCollectivite, enTete);
    listeCollectivites.push_back(nouvelleCollectivite);
    std::cout << "Nouvelle collectivité créée : " << nom << std::endl;
    mettreAJourRegions(listeCollectivites, enTete);
}

void editerCollectivite(std::vector<Collectivite>& listeCollectivites, const std::vector<std::string>& enTete) {
    std::string code;
    std::cout << "Saisissez le code de la collectivité à éditer : ";
    std::cin >> code;
    for (auto& collectivite : listeCollectivites) {
        if (collectivite.code == code) {
            recueillirInfosCollectivite(collectivite, enTete);
            std::cout << "Collectivité éditée avec succès !" << std::endl;
            mettreAJourRegions(listeCollectivites, enTete);
            return;
        }
    }
    std::cout << "Collectivité introuvable." << std::endl;
}

void fusionnerCollectivite(std::vector<Collectivite>& listeCollectivites, std::vector<std::string>enTete) {
    std::string code1, code2;
    std::cout << "Saisissez le code de la première collectivité à fusionner : ";
    std::cin >> code1;
    std::cout << "Saisissez le code de la deuxième collectivité à fusionner : ";
    std::cin >> code2;

    int index1 = -1, index2 = -1;
    for (size_t i = 0; i < listeCollectivites.size(); ++i) {
        if (listeCollectivites[i].code == code1) {
            index1 = i;
        }
        else if (listeCollectivites[i].code == code2) {
            index2 = i;
        }
    }

    if (index1 != -1 && index2 != -1) {
        for (const auto& pair : listeCollectivites[index2].nombreCollectivites) {
            listeCollectivites[index1].nombreCollectivites[pair.first] += pair.second;
        }
        listeCollectivites.erase(listeCollectivites.begin() + index2);
        std::cout << "Collectivités fusionnées avec succès !" << std::endl;
    }
    else {
        std::cout << "Une ou les deux collectivités sont introuvables." << std::endl;
    }
}

void supprimerCollectivite(std::vector<Collectivite>& listeCollectivites, const std::vector<std::string>& enTete) {
    std::string code;
    std::cout << "Saisissez le code de la collectivité à supprimer : ";
    std::cin >> code;
    for (size_t i = 0; i < listeCollectivites.size(); ++i) {
        if (listeCollectivites[i].code == code) {
            listeCollectivites.erase(listeCollectivites.begin() + i);
            std::cout << "Collectivité supprimée avec succès !" << std::endl;
            return;
        }
    }
    std::cout << "Collectivité introuvable." << std::endl;
    mettreAJourRegions(listeCollectivites, enTete);
}

int compteurFichier = 1;

void enregistrerDansFichierCSV(const std::vector<Collectivite>& listeCollectivites, std::vector<std::string>enTete) {
    //std::string nomFichier = "nouvelles_collectivites_" + std::to_string(compteurFichier) + ".csv";
    std::string nomFichier;
    std::cout << "Entrez le nom du fichier pour enregistrer : ";
    std::cin >> nomFichier;
    nomFichier += nomFichier + ".csv";
    std::ofstream fichier(nomFichier);
    if (!fichier.is_open()) {
        std::cout << "Impossible d'ouvrir le fichier CSV." << std::endl;
        return;
    }
    fichier << "Type,Code,Nom,AppartientA,Dep,Arr,EPCI,Cant,Comm\n";
    for (const auto& collectivite : listeCollectivites) {
        fichier << collectivite.type << ','
            << collectivite.code << ','
            << collectivite.nom << ','
            << collectivite.appartientA << ',';
        for (const auto& pair : collectivite.nombreCollectivites) {
            fichier << pair.second << ',';
        }
        fichier << '\n';
    }
    fichier.close();
    std::cout << "Les données ont été enregistrées dans le fichier '" << nomFichier << "'." << std::endl;
    compteurFichier++;
}

int main() {
    std::vector<Collectivite> listeCollectivites;
    std::vector<std::string>enTete;
    initialiserCollectivites(listeCollectivites, enTete);
    int option;
    std::string input;

    while (true) {
        std::cout << "---------------------------------------------------\n";
        std::cout << "Menu :\n";
        std::cout << "  1 - Afficher les détails des collectivités\n";
        std::cout << "  2 - Créer une collectivité\n";
        std::cout << "  3 - Éditer une collectivité\n";
        std::cout << "  4 - Fusionner des collectivités\n";
        std::cout << "  5 - Supprimer une collectivité\n";
        std::cout << "  6 - Enregistrer les données dans un fichier .CSV\n";
        std::cout << "  7 - Reinitialiser le tableau\n";
        std::cout << "  8 - Quitter\n";
        std::cout << "Choisissez une option : ";
        std::cin >> option;
        std::cin.ignore();

        switch (option) {
        case 1:
            std::cout << "Entrez R pour les régions, D pour les départements, T pour tous, ou le code d'une collectivité spécifique : ";
            std::cin >> input;
            std::cout << "---------------------------------------------------\n";
            if (input == "R" || input == "D" || input == "T") {
                afficherCollectivites(listeCollectivites, "", input[0]);
            }
            else {
                afficherCollectivites(listeCollectivites, input, 'T');
            }
            break;
        case 2:
            creerCollectivite(listeCollectivites, enTete);
            break;
        case 3:
            editerCollectivite(listeCollectivites, enTete);
            break;
        case 4:
            fusionnerCollectivite(listeCollectivites, enTete);
            break;
        case 5:
            supprimerCollectivite(listeCollectivites, enTete);
            break;
        case 6:
            enregistrerDansFichierCSV(listeCollectivites, enTete);
            break;
        case 7:
            initialiserCollectivites(listeCollectivites, enTete);
            break;
        case 8:
            return 0;
        default:
            std::cout << "Option non reconnue. Veuillez réessayer.\n";
        }
    }
    return 0;
}
