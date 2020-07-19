import uuid
import base64
import datetime
import random as r
import GlobalConst
import Tools

import data_items

# This is needed below for the eval method of script
from items.NormalItem import NormalItem
from items.ConsumeItem import ConsumeItem
from items.EquipItem import EquipItem

_g_items = {}

def onInit():
	for key, data in data_items.data.items():
		script = data['scriptName']
		scriptinst = eval(script)()
		_g_items[key] = scriptinst
		scriptinst.loadFromDict(data)


def getItemByIndex(index):
	return _g_items.get(index)

def getItemByID(id):
	for key, item in _g_items.items():
		if item.getID() == id:
			return item

def getNewItemByID(id):
	for key, item in _g_items.items():
		if item.getID() == id:
			newItemCopy = item.copy()
			newItemCopy.copyInto(item)
			return newItemCopy

def getItemByName(name):
	for key, item in _g_items.items():
		if item.getName() == name:
			return item

def getNewItemByName(name):
	for key, item in _g_items.items():
		if item.getName() == name:
			newItemCopy = item.copy()
			newItemCopy.copyInto(item)
			return newItemCopy

def getNewItemByUUID(uuid):
	for key, item in _g_items.items():
		if item.getUUID() == uuid:
			newItemCopy = item.copy()
			newItemCopy.copyInto(item)
			return newItemCopy

def getItemByUUID(uuid):
	for key, item in _g_items.items():
		if item.getUUID() == uuid:
			return item

def generate_uuid():
	random_string = ''
	random_str_seq = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	uuid_format = [8, 4, 4, 4, 12]
	for n in uuid_format:
		for i in range(0, n):
			random_string += str(random_str_seq[r.randint(0, len(random_str_seq) - 1)])
		if n != 12:
			random_string += '-'
	return random_string


# get a UUID - URL safe, Base64
def get_a_uuid():
	r_uuid = base64.urlsafe_b64encode(uuid.uuid4().bytes)
	return r_uuid.replace('=', '')


def getTimestamp():
	return datetime.datetime.now().strftime('%Y%m%d%H%M%S')


def getTimestampWithMs():
	return datetime.datetime.now().strftime('%Y%m%d%H%M%S%f')