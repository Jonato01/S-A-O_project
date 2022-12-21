# S-A-O_project
progetto di sistemi operativi
git clone https://github.com/Gionato65437/S-A-O_project.git

per ogni nave:
	while(tempo < max o domande azzerata){
	1:	cerca porto più vicino con merce non prenotata (ritiro) e banchina libera.
		prenota merci dal porto fino a esaurimento capienza o offerta, prenota banchina.
		raggiungi porto.
		occupa banchina.
	2: 	carica merci prenotate (ritiro).
		liberare banchina
		cerca porto più vicino con domanda di merci a bordo non prenotate (consegna) e banchina libera.
		raggiungi porto.
		occupa banchina.
		scarica merci prenotate (consegna) fino a esaurimento o soddisfacimento.
		if(porto offre roba non prenotata(consegna)){
			vai a 2:
		}
		libera banchina.
		vai a 1:
	}
	
	valore numerico di priorità nella scelta dei porti?
		un porto in cui posso sia caricare che scaricare è una scelta migliore
		rispetto a uno in cui posso compiere una sola operazione?
		
	se una nave non trova porti in cui andare?
		sta ferma e aspetta? si mette in coda?
