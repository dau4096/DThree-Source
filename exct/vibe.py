import xml.etree.ElementTree as ET
import re as regex
import discord, os.path, random
from exct.shared import replyMessage, formatName


#Genre: Metal/Rock -> ["metal", "rock"]
global multiAttribSplitChar, nameList
multiAttribSplitChar = ";"

nameList = {
	"dau": "__dau__", "d": "__dau__",
	"cesko": "tornadoteam_the_t","t": "tornadoteam_the_t", "tornado": "tornadoteam_the_t",
	"howitzer": "themightyhowitzer", "m": "themightyhowitzer", "tmh": "themightyhowitzer",
	"ace": "hamez_boi",
	"random": "randomuser78", "r": "randomuser78",
	"shabble": "shabbles", "shab": "shabbles", "femboy": "shabbles",
	"boogie": "boogie152", "boog": "boogie152",
	"melted": "meltedpuddingwx",
	"gitty": "worldofrice"
}


def splitAttr(attr: str) -> list[str]:
	#Splits by multiAttribSplitChar
	lst = attr.split(multiAttribSplitChar)
	return [element.lower().strip() for element in lst]


class Song:
	def __init__(self, songXML: ET.Element):
		#Convert XML song representation into the class for easier handling and access.
		songAttr = songXML.attrib

		#Attribs with only 1 value allowed.
		self.name = songAttr["name"]
		self.artist = songAttr["artist"]
		self.suggestedBy = songAttr["suggestedBy"]
		self.url = songAttr["link"]

		#Attribs with multiple values allowed
		self.id = splitAttr(songAttr["id"])
		self.genres = splitAttr(songAttr["genre"])
		self.intensities = splitAttr(songAttr["intensity"])
		self.moods = splitAttr(songAttr["mood"])
		self.association = splitAttr(songAttr["association"])

		try:
			#Convert duration to integer seconds. If this fails, handle somehow.
			#Allowed to be [80 | 80.0 | 80s | 80.0s].
			self.duration = int(songAttr["duration"].replace("s", ""))
		except ValueError:
			raise AttributeError("Duration must be an integer number of seconds.")

	def __repr__(self) -> str:
		#When printing to console.
		return f"<Song: [{self.name} by {self.artist}, suggested by {self.suggestedBy} | duration: {self.duration}s | genres: {self.genres} | intensity: {self.intensities} | mood: {self.moods} | url: {self.url}]>"


	def getAttribFromStr(self, attrStr: str) -> list[str]|list[int]:
		#Get attribute list from a string attribute name the user gave.
		#Some strings can mean the same attribute, depending on user wording.
		attrDict = {
			"id": self.id,
			"name": self.name, "title": self.name,
			"artist": self.artist, "musician": self.artist, "creator": self.artist, "by": self.artist,
			"addedby": self.suggestedBy, "user": self.suggestedBy,
			"genre": self.genres,
			"intensity": self.intensities,
			"mood": self.moods,
			"duration": self.duration, "length": self.duration,
			"association": self.association, "source": self.association
		}
		if attrStr in attrDict:
			#Convert attribute to list for proper external handling later.
			val = attrDict[attrStr]
			if isinstance(val, list): return val
			return [val,]
		else:
			raise AttributeError("Unknown attribute: " + attrStr)


	def format(self, noURL: bool=False) -> str:
		#Convert to nice looking discord markdown formatting
		return f"""
### *'{self.name}' by '{self.artist}' : {self.duration}s*
-# *Suggested by {formatName(self.suggestedBy)}*
{'' if noURL else self.url}"""


def handleDuration(thisSong: Song, searchAttrib: str, searchValue: int) -> bool:
	if searchAttrib in ("lessthan", "less"):
		return thisSong.getAttribFromStr("duration")[0] <= searchValue
	elif searchAttrib in ("morethan", "more"):
		return thisSong.getAttribFromStr("duration")[0] >= searchValue
	else:
		return False



def handleAttribList(thisSong: Song, searchAttrib: str, searchValue: str) -> bool:
	if searchAttrib in ("lessthan", "less", "morethan", "more"): return False
	return any(attr == searchValue for attr in thisSong.getAttribFromStr(searchAttrib))



def findRelevantSongs(messageData: str, username: str) -> list[Song]:
	#Load XML file
	tree = ET.parse("/opt/render/project/src/textFiles/vibe/songData.xml")
	root = tree.getroot()


	#Clean input.
	inputStr = messageData.replace("/vibe", "").strip().lower()
	inputSplit = inputStr.split(" ")


	allSongs = False
	randomSong = False
	searchAttrib, searchValue = "", ""
	if (len(inputSplit) < 1) or (len(inputSplit) == 1) and (inputSplit[0] == ""):
		#Must just be "/vibe" alone; show all songs.
		randomSong = True
		allSongs = True

	elif (len(inputSplit) >= 2):
		#Must have some attribute to search for.
		searchAttrib = inputSplit[0]
		searchValue = " ".join(inputSplit[1:])
		if searchAttrib in ("lessthan", "less", "morethan", "more"):
			try:
				#Allowed to be [80 | 80.0 | 80s | 80.0s].
				searchValue = int(searchValue.replace("s", ""))
			except ValueError:
				raise ValueError("Duration must be an integer number of seconds.")

		if (searchAttrib in ("addedby", "user")) :
			if (searchValue in nameList):
				searchValue = nameList[searchValue]
			elif (searchValue in ("me", "myself")):
				searchValue = username

	else:
		if inputSplit[0] in ("all", "everything"):
			#Allow specifying "all" songs.
			allSongs = True
		else:
			#Invalid format; Cannot ask for an attribute without specifying a value.
			raise ValueError("Invalid input: " + messageData)


	songList = []
	for songXML in root:
		#Iterate through all songs in the xml file and convert them to a class; then select which are relevant.
		#Could be optimised by finding relevant matches first then converting?
		thisSong = Song(songXML)
		if allSongs or handleDuration(thisSong, searchAttrib, searchValue) or handleAttribList(thisSong, searchAttrib, searchValue):
			songList.append(thisSong)


	if randomSong:
		return [random.choice(songList),]
	else:
		return songList



async def showSongs(message: discord.Message, messageData: str) -> None:
	#Example: "/vibe all"
	songList = findRelevantSongs(messageData, str(message.author))

	if len(songList) == 0:
		finalMsgString = "Could not find any relevant songs."
	else:
		finalMsgString = ""
		for song in songList:
			noURL = regex.search(r"^[^/].+\.(mp4|mp3|avi|m4a)$", song.url) is not None
			finalMsgString += song.format(noURL=noURL)
			if noURL:
				try:
					await message.channel.send(file=discord.File("/opt/render/project/src/textFiles/vibe" + song.url))
				except FileNotFoundError:
					finalMsgString += "Could not find relevent file."



	await replyMessage(message, finalMsgString)
