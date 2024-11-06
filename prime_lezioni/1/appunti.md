## Introduzione

Perché programmazione parallela?

    I processore aumenta di due volte all'anno di prestazioni.
    dal 2003 però la legge di Moore è finita e all'anno aumenta del 4% l'efficienza.

    Quindi invece che fare processori più performanti si è deciso di fare più CPU più piccole, ma concorrenti.

    La frequenza delle CPU hanno smesso di aumentare a livello di mercato,mentre il numero di core
    su una singola CPU sono aumentati e il numero di transistori hanno continuato ad aumentare esponenzialmente.

    I programmi seriali non sfrutta al 100% la CPU perché gira su solo un core.

Perché abbiamo bisogno di tutta quwesta potenza di calcolo?

    Questo per risolvere problemi sempre più complessi, come:
    - creare LLM.
    - decodificare il genoma umano.
    - modellazione climatica.
    - ecc...


    Quindi tradizionalmente la performace arrivava dall'aumento della densità dei transistor.
    Però si è arrivati a delle barriere fisiche, come gestire la temperatura che potrebbero flippare dei bit.

    I programmi devono scrivere parallelamente esplicitamente, quindi questa programmazione è
    molto diversa.

    Si è provato a rendere automatica questa programmazione, ma ancora è molto limitata.

    Alcuni algoritmi sequenziali non è facile convertirli in paralleli e spesso bisogna ripensare al problema da capo.

Come scrivo codice parallelo?

    Un modo per il "Task parallelism". Abbiamo delle task e le distribuiamo fra i vari core.

    Oppure il "Data parallism". Vengono partizionati i dati e i vari core fanno la stessa cosa, ma con altri dati.

    Supponiamo di comprimere 100 file.
    Potrei dire che ogni core mi decomprime 10 file.
    Però ad un certo punto si devono sincronizzare per vari motivi.
    Magare un core deve comunicare con un altro core, oppure per bilanciare e che un core non faccia troppo più lavoro degli altri e quindi i core si possono scambarsi il carico.
    Oppure i core si devono sincronizzare.

