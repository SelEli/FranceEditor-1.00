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
#include <locale>

std::map<std::string, std::string> errorMessages = {
    {"FileNotFound", "\033[31m" + std::string("--> ERREUR : Fichier non trouvé.") + "\033[0m"},
    {"FileOpeningImpossible", "\033[31m" + std::string("-->  ERREUR : Impossible d'ouvrir le fichier.") + "\033[0m"},
    {"DataNotFound", "\033[31m" + std::string("-->  ERREUR : Données non trouvées.") + "\033[0m"},
    {"DataAlreadyExists", "\033[31m" + std::string("-->  ERREUR : Données déjà existantes.") + "\033[0m"},
    {"InvalidChoice", "\033[31m" + std::string("-->  ERREUR : Choix invalide.") + "\033[0m"},
};

void printErrorMessage(const std::string& errorType) {
    if (errorMessages.count(errorType) > 0) {
        std::cout << errorMessages[errorType] << std::endl;
    }
    else {
        std::cout << "Erreur inconnue." << std::endl;
    }
}

std::map<std::string, std::string> successMessages = {
    {"DataSaved", "\033[32m" + std::string("--> SUCCES : Données enregistrées.") + "\033[0m"},
    {"DataCreated", "\033[32m" + std::string("--> SUCCES : Données créées.") + "\033[0m"},
    {"DataEdited", "\033[32m" + std::string("--> SUCCES : Données éditées.") + "\033[0m"},
    {"DataDeleted", "\033[32m" + std::string("--> SUCCES : Données supprimées.") + "\033[0m"},
};

void printSuccessMessage(const std::string& successType) {
    if (successMessages.count(successType) > 0) {
        std::cout << successMessages[successType] << std::endl;
    }
    else {
        std::cout << "Opération réussie." << std::endl;
    }
}

class Collectivite {
public:
    char type;
    std::string code;
    std::string nom;
    std::string appartientA;
    std::map<std::string, int> nombreCollectivites;
    std::vector<std::string> ordreCollectivites;
    bool affiche;

    Collectivite(char type, std::string code, std::string nom, std::string appartientA)
        : type(type), code(code), nom(nom), appartientA(appartientA), affiche(false) {}

    void ajouterTypeCollectivite(const std::string& typeCollectivite, int nombre = 0) {
        nombreCollectivites[typeCollectivite] = nombre;
        ordreCollectivites.push_back(typeCollectivite);
    }

    void modifierTypeCollectivite(const std::string& ancienNom, const std::string& nouveauNom) {
        auto it = nombreCollectivites.find(ancienNom);
        if (it != nombreCollectivites.end()) {
            int nombre = it->second;
            nombreCollectivites.erase(it);
            nombreCollectivites[nouveauNom] = nombre;
            std::replace(ordreCollectivites.begin(), ordreCollectivites.end(), ancienNom, nouveauNom);
        }
        else {
            printErrorMessage("DataNotFound");
        }
    }

    void supprimerTypeCollectivite(const std::string& typeCollectivite) {
        auto it = nombreCollectivites.find(typeCollectivite);
        if (it != nombreCollectivites.end()) {
            nombreCollectivites.erase(it);
            ordreCollectivites.erase(std::remove(ordreCollectivites.begin(), ordreCollectivites.end(), typeCollectivite), ordreCollectivites.end());
        }
        else {
            printErrorMessage("DataNotFound");
        }
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
        std::cout << "  Type de collectivité : " << type << std::endl;
        std::cout << "  Code de la collectivité : " << code << std::endl;
        std::cout << "  Nom de la collectivité : " << nom << std::endl;
        std::cout << "  Appartient à : " << appartientA << std::endl;
        for (const auto& key : ordreCollectivites) {
            std::cout << "  " << key << " : " << nombreCollectivites.at(key) << std::endl;
        }
        std::cout << std::endl;
    }

    static void enregistrerDansCSV(const std::vector<Collectivite>& listeCollectivites, const std::string& nomFichier) {
        std::ofstream fichier(nomFichier);
        if (!fichier.is_open()) {
            printErrorMessage("FileOpeningImpossible");
            return;
        }

        fichier << "Type,Code,Nom,Appartient a";
        if (!listeCollectivites.empty()) {
            for (const auto& typeCollectivite : listeCollectivites[0].ordreCollectivites) {
                fichier << "," << typeCollectivite;
            }
        }
        fichier << "\n";

        for (const auto& collectivite : listeCollectivites) {
            fichier << collectivite.type << "," << collectivite.code << "," << collectivite.nom << "," << collectivite.appartientA;
            for (const auto& typeCollectivite : collectivite.ordreCollectivites) {
                fichier << "," << collectivite.nombreCollectivites.at(typeCollectivite);
            }
            fichier << "\n";
        }

        fichier.close();
        printSuccessMessage("DataSaved");
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
        std::cout << "Voulez-vous créer un tableau manuellement (M) ou à partir d'un fichier CSV (C), ou retourner au menu (Q) ? ";
        std::cin >> choix;
        if (choix == 'Q' || choix == 'q') {
            return;
        }
        else if (choix == 'M' || choix == 'm' || choix == 'C' || choix == 'c') {
            std::cout << "Attention : cela va reinitialiser le tableau entièrement. Etes-vous sur ? (O/N) ";
            char confirmer;
            std::cin >> confirmer;
            if (confirmer == 'O' || confirmer == 'o') {
                listeCollectivites.clear();
                if (choix == 'M' || choix == 'm') {
                    std::cout << "Combien de categories de collectivites voulez-vous ajouter ? ";
                    int nbCategorieColl = 0;
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
                    printSuccessMessage("DataCreated");
                    return;
                }
                else if (choix == 'C' || choix == 'c') {
                    std::cout << "Entrez le nom du fichier CSV : ";
                    std::string nomFichier;
                    std::cin >> nomFichier;
                    nomFichier = nomFichier + ".csv";
                    std::ifstream fichier(nomFichier);
                    if (!fichier.is_open()) {
                        printErrorMessage("FileOpeningImpossible");
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
                    printSuccessMessage("DataCreated");
                    return;
                }
                else {
                    printErrorMessage("InvalidChoice");
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
        if (collectivite.type == 'R') {
            int total = 0;
            for (const auto& autreCollectivite : listeCollectivites) {
                if (autreCollectivite.type == 'D' && autreCollectivite.appartientA == collectivite.code) {
                    total += 1;
                }
            }
            collectivite.nombreCollectivites["Dep"] = total;
        }
    }
}


void saisirCode(std::string& code1) {
    std::cout << "Saisissez le code de la collectivité : ";
    std::cin >> code1;
}

int trouverCollectivite(const std::vector<Collectivite>& listCommunity, const std::string& code1) {
    for (size_t i = 0; i < listCommunity.size(); ++i) {
        if (listCommunity[i].code == code1) return i;
    }
    return -1;
}

void gererCollectivite(std::vector<Collectivite>& listCommunity, std::vector<std::string>& enTete, std::string op) {
    std::string code1, code2;
    int index;

    if (op == "add") {
        Collectivite nouvelleCollectivite('R', code1, "", "");
        recueillirInfosCollectivite(nouvelleCollectivite, enTete);
        listCommunity.push_back(nouvelleCollectivite);
        printSuccessMessage("DataCreated");
        mettreAJourRegions(listCommunity, enTete);
    }
    else if (op == "edit" || op == "delete") {
        saisirCode(code1);
        index = trouverCollectivite(listCommunity, code1);

        if (index == -1) {
            printErrorMessage("DataNotFound");
            return;
        }

        if (op == "edit") {
            recueillirInfosCollectivite(listCommunity[index], enTete);
            printSuccessMessage("DataEdited");
        }
        else {
            listCommunity.erase(listCommunity.begin() + index);
            printSuccessMessage("DataDeleted");
        }

        mettreAJourRegions(listCommunity, enTete);
    }
    else if (op == "merge") {
        std::cout << "Saisissez le code de la première collectivité à fusionner : ";
        std::cin >> code1;
        std::cout << "Saisissez le code de la deuxième collectivité à fusionner : ";
        std::cin >> code2;
        int index1 = -1, index2 = -1;
        for (size_t i = 0; i < listCommunity.size(); ++i) {
            if (listCommunity[i].code == code1) {
                index1 = i;
            }
            else if (listCommunity[i].code == code2) {
                index2 = i;
            }
        }


        if (index1 != -1 && index2 != -1) {
            for (const auto& pair : listCommunity[index2].nombreCollectivites) {
                listCommunity[index1].nombreCollectivites[pair.first] += pair.second;
            }
            listCommunity.erase(listCommunity.begin() + index2);
            std::cout << "Collectivités fusionnées avec succès !" << std::endl;
        }
        else {
            std::cout << "Une ou les deux collectivités sont introuvables." << std::endl;
        }

        mettreAJourRegions(listCommunity, enTete);
    }
}

void gererTypeCollectivite(std::vector<Collectivite>& listCommunity, std::vector<std::string>& enTete, std::string op) {
    std::string nom, input;
    if (op == "add") {
        std::cout << "Entrez le nom du type de collectivité à ajouter : ";
        std::cin >> nom;
        for (auto& collectivite : listCommunity) {
            collectivite.ajouterTypeCollectivite(nom);
        }
        printSuccessMessage("DataCreated");
    }
    else if (op == "edit") {
        std::cout << "Entrez le nom actuel du type de collectivité à éditer : ";
        std::cin >> input;
        std::cout << "Entrez le nouveau nom pour ce type de collectivité : ";
        std::cin >> nom;
        for (auto& collectivite : listCommunity) {
            collectivite.modifierTypeCollectivite(input, nom);
        }
        printSuccessMessage("DataEdited");
    }
    else if (op == "delete") {
        std::cout << "Entrez le nom du type de collectivité à supprimer : ";
        std::cin >> nom;
        for (auto& collectivite : listCommunity) {
            collectivite.supprimerTypeCollectivite(nom);
        }
        printSuccessMessage("DataDeleted");
    }
}


/*
void saisirCode(std::string& code1) {
    std::cout << "Saisissez le code de la collectivité : ";
    std::cin >> code1;
}

int trouverCollectivite(const std::vector<Collectivite>& listeCollectivites, const std::string& code1) {
    for (size_t i = 0; i < listeCollectivites.size(); ++i) {
        if (listeCollectivites[i].code == code1) {
            return i;
        }
    }
    return -1;
}

void gererCollectivite(std::vector<Collectivite>& listeCollectivites, std::vector<std::string>& enTete, std::string operation) {
    //std::string code;
    std::string code1, code2;
    int index;

    if (operation == "add") {
        char type = 'R';
        //std::string code;
        std::string nom;
        std::string appartientA;
        Collectivite nouvelleCollectivite(type, code1, nom, appartientA);
        recueillirInfosCollectivite(nouvelleCollectivite, enTete);
        listeCollectivites.push_back(nouvelleCollectivite);
        printSuccessMessage("DataCreated");
        mettreAJourRegions(listeCollectivites, enTete);
    }
    else if (operation == "edit" || operation == "delete") {
        saisirCode(code1);
        index = trouverCollectivite(listeCollectivites, code1);

        if (index == -1) {
            printErrorMessage("DataNotFound");
            return;
        }

        if (operation == "edit") {
            recueillirInfosCollectivite(listeCollectivites[index], enTete);
            printSuccessMessage("DataEdited");
        }
        else if (operation == "delete") {
            listeCollectivites.erase(listeCollectivites.begin() + index);
            printSuccessMessage("DataDeleted");
        }

        mettreAJourRegions(listeCollectivites, enTete);
    }
    else if (operation == "merge") {
        //std::string code1, code2;
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
    }
}

void gererTypeCollectivite(std::vector<Collectivite>& listeCollectivites, std::vector<std::string>& enTete, std::string operation) {
    std::string nom;
    std::string input;
    if (operation == "add") {
        std::cout << "Entrez le nom du type de collectivité à ajouter : ";
        std::cin >> nom;
        for (auto& collectivite : listeCollectivites) {
            collectivite.ajouterTypeCollectivite(nom);
        }
        printSuccessMessage("DataCreated");
    }
    else if (operation == "edit") {
        std::cout << "Entrez le nom actuel du type de collectivité à éditer : ";
        std::cin >> input;
        std::cout << "Entrez le nouveau nom pour ce type de collectivité : ";
        std::cin >> nom;
        for (auto& collectivite : listeCollectivites) {
            collectivite.modifierTypeCollectivite(input, nom);
        }
        printSuccessMessage("DataEdited");
    }
    else if (operation == "delete") {
        std::cout << "Entrez le nom du type de collectivité à supprimer : ";
        std::cin >> nom;
        for (auto& collectivite : listeCollectivites) {
            collectivite.supprimerTypeCollectivite(nom);
        }
        printSuccessMessage("DataDeleted");
    }
}*/

void afficherAide() {
    std::cout << "\n  Programme de visualisation et d'édition des collectivités territoriales\n\n";
    std::cout << "  Détail des options du MENU\n\n";
    std::cout << "  Option  1 : A chaque DEMARRAGE du programme, il faut au préalable créer un tableau des collectivités.\n";
    std::cout << "  Option  2 : Il est possible d'afficher soit une, soit plusieurs collectivités.\n";
    std::cout << "  Option  3 : Le programme ne peut créer que 2 niveaux détaillés de collectivités, R ou D.\n";
    std::cout << "  Option  4 : L'édition de collectivité ne modifie que les nombres.\n";
    std::cout << "  Option  5 : La fusion de collectivité se fait en additionnant les nombres.\n";
    std::cout << "  Option  6 : La suppression met à jour le tableau après soustraction des nombres.\n";
    std::cout << "  Option  7 : L'ajout du TYPE de collectivité place une nouvelle colonne en fin de tableau.\n";
    std::cout << "  Option  8 : L'édition du TYPE de collectivité modifie le nom pour toutes les collectivités du tableau.\n";
    std::cout << "  Option  9 : La suppression du TYPE de collectivité fait disparaître son nom et ses nombres associés.\n";
    std::cout << "  Option 10 : L'enregistrement est suggéré sous un nouveau nom pour ne pas écraser un fichier existant.\n";
    std::cout << "  Option 11 : Affichage de l'aide actuelle...\n";
    std::cout << "  Option 12 : Cette option quitte le programme et perd les modifications non-enregistrées dans un fichier !\n\n";
}

void quine() {
    std::ifstream file(__FILE__);
    std::string line;

    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
}

int main() {
    std::locale::global(std::locale(""));
    std::vector<Collectivite> listeCollectivites;
    std::vector<std::string>enTete;
    int option = 1;
    std::string op;
    std::string input;
    std::string separateur = "       ---------------------------------------       \n";

    while (true) {
        std::cout << "\n" << separateur;
        std::cout << "                       MENU                          \n";
        std::cout << separateur;
        std::cout << "  1  - INITIALISER un tableau des collectivités\n";
        std::cout << separateur;
        std::cout << "  2  - AFFICHER les détails des collectivités\n";
        std::cout << "  3  -    Créer une collectivité\n";
        std::cout << "  4  -    Éditer une collectivité\n";
        std::cout << "  5  -    Fusionner des collectivités\n";
        std::cout << "  6  -    Supprimer une collectivité\n";
        std::cout << separateur;
        std::cout << "  7  -    Ajouter un type de collectivité\n";
        std::cout << "  8  -    Editer un type de collectivité\n";
        std::cout << "  9  -    Supprimer un type de collectivité\n";
        std::cout << separateur;
        std::cout << "  10 - ENREGISTRER les données dans un fichier .CSV\n";
        std::cout << separateur;
        std::cout << "  11 - AIDE\n";
        std::cout << "  12 - QUITTER\n";
        std::cout << separateur;
        std::cout << "  Choisissez une option : ";
        std::cin >> option;
        std::cin.ignore();
        std::cout << separateur;

        switch (option) {
        case 1:
            initialiserCollectivites(listeCollectivites, enTete);
            break;
        case 2:
            std::cout << "Entrez R pour les régions, D pour les départements, T pour tous, ou le code d'une collectivité spécifique : ";
            std::cin >> input;
            std::cout << separateur;
            if (input == "R" || input == "r" || input == "D" || input == "d" || input == "T" || input == "t") {
                afficherCollectivites(listeCollectivites, "", input[0]);
            }
            else {
                afficherCollectivites(listeCollectivites, input, 'T');
            }
            break;
        case 3:
            gererCollectivite(listeCollectivites, enTete, "add");
            break;
        case 4:
            gererCollectivite(listeCollectivites, enTete, "edit");
            break;
        case 5:
            gererCollectivite(listeCollectivites, enTete, "merge");
            break;
        case 6:
            gererCollectivite(listeCollectivites, enTete, "delete");
            break;
        case 7:
            gererTypeCollectivite(listeCollectivites, enTete, "add");
            break;
        case 8:
            gererTypeCollectivite(listeCollectivites, enTete, "edit");
            break;
        case 9:
            gererTypeCollectivite(listeCollectivites, enTete, "delete");
            break;
        case 10:
            std::cout << "Entrez le nom du fichier CSV dans lequel vous voulez enregistrer les données : ";
            std::cin >> input;
            Collectivite::enregistrerDansCSV(listeCollectivites, input + ".csv");
            break;
        case 11:
            afficherAide();
            break;
        case 12:
            return 0;
        case 13:
            quine();
            break;

        default:
            printErrorMessage("InvalidChoice");
        }
    }
}
