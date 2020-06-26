# SO2_Pr_Threads
Problem synchronizacji wątków w C++.

# Temat: Układ Krwionośny.
Układ krwionośny i odpornościowy:
	- Wątki:
		- płuca - cykle wdech / wydech - pobieranie tlenu i przekazywanie go erytrocytom
		- serce - porusza krwinki
		- erytrocyty - transport tlenu z płuc do komórek
		- leukocyty - "patrolowanie" organizmu w poszukiwaniu bakterii atakujących komórki
					- pomoc komórce zaatakowanej przez bakterię
		- bakterie - losowo atakują komórki
		- komórki - do każdej komórki muszą regularnie trafiać erytrocyty z tlenem,
				- mogą być atakowane przez bakterie, wtedy muszą trafić do nich leukocyty w celu obrony
	- Zasoby:
		- tlen - jedna jednostka tlenu z płuc może być zabrana przez jeden erytrocyt
		- żyły - ograniczona liczba krwinek w przekroju poprzecznym, krwinki nie mogą ze sobą kolidować
				- mają wymusić rozproszenie krwinek w całym układzie
	- Rząd wielkości liczby wystąpień wątków:
		- płuca - jeden
		- serce - jeden
		- erytrocyty - kilka - kilkanaście 
		- leukocyty - kilka 
		- bakterie - kilka
		- komórki - kilka
	- Opis systemu:
	System składa się z płuc, komórek i żył. Żyły łączą płuca i komórki, tworząc zamknięty obieg cykliczny.
	Każda komórka wymaga regularnych dostaw tlenu z płuc.
	Za transport tlenu odpowiadają erytrocyty, które krążą w żyłach.
	Każdy erytrocyt może pobrać jendostkę tlenu z płuc i przekazać ją komórce.
	Komórki w układzie mogą być atakowane przez bakterie - wymaga to interwencji leukocytów, które krążą w żyłach razem z erytrocytami.
