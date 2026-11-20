#ifndef CALCULATEUR_LEQ_H
#define CALCULATEUR_LEQ_H

#include <Arduino.h>
#include "calculateur_li.h"
#include "calculateur_vrms.h"

class Calculateur_Leq {
  private:
    float m_leq;                        // Niveau d'énergie sonore au temps ti
    double m_ts;                        // Période de temps tp
    Calculateur_Li d;                   // Objet de classe Calculateur_Li
    double m_somme = 0.0;
    int m_nb_li;
    int m_nb_sample;
    uint32_t t = 0;
    uint32_t t2 = 0;
    uint32_t t3 = 0;
    uint32_t m_nb_total_sample = 0;


  public:
    Calculateur_Leq(double ts = 62, int nb_sample = 32, int nb_li = 150) : m_ts(ts), m_nb_sample(nb_sample), m_nb_li(nb_li){}

    // Empêcher l'utilisation du constructeur de copie
    Calculateur_Leq(const Calculateur_Leq& other) = delete;
    // Empêcher l'utilisation de l'opérateur d'assignation
    Calculateur_Leq& operator=(const Calculateur_Leq& other) = delete;
    // Empêcher l'utilisation du constructeur par déplacement
    Calculateur_Leq(Calculateur_Leq&& other) = delete;
    // Empêcher l'utilisation de l'opérateur de déplacement
    Calculateur_Leq& operator=(Calculateur_Leq&& other) = delete;

    ~Calculateur_Leq() {}                    // Destructeur

    /* -------------------------------------------------------------
       Accesseurs des données membres de la classe
       -------------------------------------------------------------- */
    inline uint32_t GetNbSamples() const { return d.GetNbSamples(); }
    inline uint32_t GetTotalSamples() const { return d.GetTotalSamples(); }
    inline double GetVrms() const { return d.GetVrms(); }
    inline double GetdBV() const { return d.GetdBV(); }
    inline uint8_t GetAPin() const { return d.GetAPin(); }
    inline double GetVMax() const { return d.GetVMax(); }
    inline int16_t GetAdcMax() const { return d.GetAdcMax(); }

    inline double GetLi() const { return d.GetLi(); }
    inline double GetP() const { return d.GetP(); }
    inline double GetM() const { return d.GetM(); }
    inline double GetG() const { return d.GetG(); }

    /* -------------------------------------------------------------*/

    inline float GetLeq() const { return m_leq; }
    inline float GetTs() const { return m_ts; }
    inline float GetVrmSamples() const { return m_nb_sample; }
    inline float GetLiSamples() const { return m_nb_li; }

    void Accumulate() {
        if(t == 0){
            t = millis();
        }
        if(t2 == 0){
            t2 = millis();
        }
        if (millis() < t + m_ts) {
            return;
        }
        t=0;
        MAX4466_ErrorCode errorCode = d.Accumulate();
        if(errorCode != MAX4466_ErrorCode::MAX_OK){
            Serial.print("Error micro : "); Serial.println(static_cast<int>(errorCode));
            t = 0; t2 = 0; t3 = 0; 
            uint32_t t{millis()};
            while (millis() < t + 2000) {}
            return;
        }
        if (millis() < t2 + m_ts*m_nb_sample) {
            return;
        }
        t2=0;
        //m_nb_total_sample += d.GetNbSamples();
        d.Compute();
        m_somme += m_ts*m_nb_sample*pow(10, 0.1*d.GetLi()); // m_nb_total_sample*m_ts
        //Serial.print("Li : "); Serial.println(d.GetLi());
    }

    bool Compute() {
        if(t3 == 0){
            t3 = millis();
        }
        if (millis() < t3 + m_ts*m_nb_sample*m_nb_li) {
            return false;
        }
        t3=0;
        m_leq = 10.0 * log10(1.0/(m_ts*m_nb_sample*m_nb_li) * m_somme);
        m_somme = 0.0;
        //m_nb_total_sample = 0;
        return true;

    }
};

#endif