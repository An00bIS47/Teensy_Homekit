
python3 -m homekit.pair -d 04:E9:E5:0C:D8:51 -p 031-45-712 -f ./homekitStorage.json -a t41 --log DEBUG


python3 -m homekit.get_accessories -f ./homekitstorage.json -a t41 --log DEBUG

python3 -m homekit.get_characteristic -f ./homekitstorage.json -a t41 -c 2.10 --log DEBUG

python3 -m homekit.remove_pairing -f ./homekitstorage.json -a t41 --log DEBUG

python3 -m homekit.get_events -f ~/homekitstorage.json -a t41 -c 2.10 --log DEBUG



for i in {1..30}; do echo $i && python3 -m homekit.get_accessories -f ./homekitstorage.json -a t41; done

for i in {1..10}; do 
	echo $i && 
	python3 -m homekit.pair -d 04:E9:E5:0C:D8:51 -p 031-45-712 -f ./homekitStorage.json -a t41 && 
	python3 -m homekit.get_accessories -f ./homekitstorage.json -a t41 &&
	python3 -m homekit.get_characteristic -f ./homekitstorage.json -a t41 -c 2.10 &&	
	python3 -m homekit.remove_pairing -f ./homekitstorage.json -a t41
; done && 
python3 -m homekit.pair -d 04:E9:E5:0C:D8:51 -p 031-45-712 -f ./homekitStorage.json -a t41 --log DEBUG &&
python3 -m homekit.get_events -f ~/homekitstorage.json -a t41 -c 2.10 --log DEBUG