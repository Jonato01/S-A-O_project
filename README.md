 # S-A-O_project
progetto di sistemi operativi
git clone https://github.com/Gionato65437/S-A-O_project.git
importante sizeof(struct shared_data)+(sizeof(struct porto)+sizeof(struct merce)*2*MERCI_RIC_OFF)*SO_PORTI+(sizeof(struct merce))*SO_MERCI+sizeof(pid_t)*SO_NAVI
per ogni nave:
	while(tempo < max o domande azzerata){
	1:	cerca porto più vicino con domanda di merce non prenotata e banchina libera.
		prenota merci dal porto fino a esaurimento capienza o domanda.
		cerca offerta in un altro parto per easudire la domanda
		trova un porto, prenota le merci e raggiungilo.
		occupa banchina.(usare semaforo)
	2: 	carica merci prenotate (ritiro).
		liberare banchina
		ritorna al porto di partenza (consegna).
		raggiungi porto.
		occupa banchina.
		scarica merci prenotate (consegna) fino a soddisfacimento.
		libera banchina.
	}
	
	valore numerico di priorità nella scelta dei porti?
		un porto in cui posso sia caricare che scaricare è una scelta migliore
		rispetto a uno in cui posso compiere una sola operazione?
	se una nave non trova porti in cui andare?
		sta ferma e aspetta? si  si mette in coda? sni



ogni nave easudisce una o più richieste ma caricando da un singolo porto





