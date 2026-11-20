
import requests, time, sys

# write
THINGSPK_URL = 'https://api.thingspeak.com/update'
THINGSPK_API_KEY = 'CKOO1DCJ91N6SKG6'

# delete
THINGSPK_CHANNEL_ID = '3518106'
THINGSPK_URL_DEL = 'https://api.thingspeak.com/channels/' + THINGSPK_CHANNEL_ID + '/feeds.json'
# Utiliser le USER API Key qui se trouve dans votre profile
THINGSPK_USER_API_KEY = 'VCEV84HFLXAWLPSW'


class CommThingSpeak : 

    data = []

    def __init__(self): 
        pass

    def addOneData(self, p_data) : 
        self.data.append(p_data)

    def addDatas(self, p_data) : 
        self.data = p_data
        print(self.data)

    def resetData(self):
        self.data = []



    def sendData(self):
    # data : tableau de 2 elements
    # Note: f-string disponible depuis Python 3.6
    #print(f'Envoyer {len(data)} données dans les {len(data[0])} champs du canal')
    #for d in data:
        try:
            print(f'Écrire {self.data[0]} et {self.data[1]} et {self.data[2]} dans les champs...', end=' ')         
            resp = requests.get(THINGSPK_URL,
                                # 10 secondes pour connection et read timeout
                                timeout = (10, 10),
                                # Parmaètres de cette requête
                                params = { "api_key" : THINGSPK_API_KEY,
                                            "field1"  : self.data[0],
                                            "field2"  : self.data[1], 
                                            "field3"  : self.data[2]}
                                )

            print(f"ThingSpeak GET response: {resp.status_code}")
            # Vérifier la réponse de ThingSpeak
            if resp.status_code != 200:
                print("Erreur de communication détectée!")

            # Attendre 20 secondes (licence gratuite a un délai de 15 secondes)
            #time.sleep(DELAY)

        except requests.ConnectionError:
            print('Erreur de connexion')
            return
        except requests.Timeout:
            print("Exception de timeout reçue (connexion ou écriture)")
            return
        except requests.HTTPError:
            print('Erreur au niveau du protocole HTTP')
            return



    def deleteFeed(self):
        # Note: f-string disponible depuis Python 3.6
        print(F'Effacer les donnnés du canal {THINGSPK_CHANNEL_ID}')
        try:
            resp = requests.delete(THINGSPK_URL_DEL,
                                # 10 secondes pour connection et read timeout
                                timeout = (10, 10),
                                # Paramètres de cette requête
                                params = { "api_key" : THINGSPK_USER_API_KEY}
                                )
            print(f'La requête est:\n{resp.url}')
            print("ThingSpeak response: {resp.status_code}")
            if resp.status_code != 200:
                print("Erreur de communication détectée!")
                sys.exit(1)
            # Afficher la réponse de ThingSpeak 
            # Voir le document "Représentation JSON" pour un survol
            try:
                print('La réponse de ThingSpeak...')
                print(resp.json())
            except ValueError:
                print('Exception ValueError reçue (décodage JSON)')
                sys.exit(2)
        except requests.ConnectionError:
            print('Erreur de connexion')
            sys.exit(3)         # 3 = Erreur de connexion
        except requests.Timeout:
            print("Exception de timeout reçue (connexion ou écriture)")
            sys.exit(4)         # 4 = Erreur de timeout
        except requests.HTTPError:
            print('Erreur au niveau du protocole HTTP')
            sys.exit(5)         # 5 = Erreur HTTP



