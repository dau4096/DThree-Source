import discord, asyncio, random, importlib, os, subprocess, datetime, time
from dotenv import load_dotenv
load_dotenv()
import signal, sys, threading;
from games.chess import checkChessGames, testImage
from games.noughtsAndCrosses import checkNoughtsAndCrossesGames
from exct.responses import checkReplies, replyToCorrection
from exct.memeBrowse import browseMemes
from exct.webSearch import lookUp
from exct.shared import removeNonASCII, getTime, sendMessage, replyMessage, timeSinceStr, sendMessageInChannel
from exct.shared import pullBackupData, backupData, updateRepo
from exct.artbook import parseIndexFile
import games.economy
import games.BlackjackD3

global D3StartTime, DTHREE_PUBLIC, client


intents = discord.Intents.default()
intents.message_content = True
intents.members = True
intents.guilds = True
intents.presences = True
client = discord.Client(intents=intents)

shutdownEvent = asyncio.Event();



#Handle incoming signals (SIGUSER1, HTTP requests)
def handleShutdown(signum, frame) -> None:
	#Shutdown via management program. Non-negotiable, but allows time for graceful closure.
	games.economy.writeCSV(f"{os.getenv('DISK_DIR')}/data/econ.csv");
	backupData(False); #Don't show anything

	sys.exit(0);

signal.signal(signal.SIGUSR1, handleShutdown);

#HTTP
def cmdListern():
	loop = asyncio.new_event_loop()
	asyncio.set_event_loop(loop)
	for line in sys.stdin:
		cmd = line.strip()
		loop.run_until_complete(cmdHandle(cmd))

async def cmdHandle(cmd:str) -> None:
	result = "";
	match (cmd):
		case "fetch":
			await updateRepo(None);
			parseIndexFile();
			result = "Successfully pulled textFile data.";

		case "push":
			backupData();
			result = "Successfully pushed ~/disk/data";

		case "pull":
			pullBackupData();
			result = "Successfully pulled ~/disk/data";

		case _:
			result = f"Unknown command: {cmd}";

	print(result, flush=True);

listener = threading.Thread(target=cmdListern, daemon=True);
listener.start();



#Background Tasks.
async def backgroundActions(client: discord.Client) -> None:
	global D3StartTime
	D3StartTime = time.time()
	try:
		#Get latest datafiles.
		pullBackupData() 
		await updateRepo(None)
		parseIndexFile()

		while not shutdownEvent.is_set():
			await asyncio.sleep(3600) #60*60s, 1 hour.
			backupData() #Backup /project/src/disk/data/


	except Exception as e:
		#Send background errors to testing server.
		return
		"""
		await sendMessageInChannel(
			client,
			f"# Error occurred in background actions: {e}\n-# @663451560465924097", #Pings __dau__
			"Dau's Repository",
			"bot-testing"
		)
		"""
		print(e)


async def shutdown(loop, tasks):
	shutdownEvent.set();

	for task in tasks:
		task.cancel();

	await asyncio.gather(*tasks, return_exceptions=True);

	loop.stop();



@client.event
async def on_ready() -> None:
	#Should it do something when deployed?
	pass


async def otherTasks(message: discord.Message, messageData: str) -> None:
	global D3StartTime, DTHREE_PUBLIC

	"""Handles all other asynchronous tasks."""
	spainFilePath = f"{os.getenv('DISK_DIR')}/data/wordsSinceSpanishInquisition.txt"
	if os.path.exists(spainFilePath):
		with open(spainFilePath, "r") as spainFile:
			lines = spainFile.readlines()
			if len(lines) > 0:
				wordsSinceSpanishInquisition = int(lines[0].strip())
				wordsSinceSpanishInquisition += 1
				if wordsSinceSpanishInquisition > 1023:
					if (random.randint(0, 1023) == 127) or (wordsSinceSpanishInquisition > 2047):
						await message.reply(file=discord.File(f"{os.getenv('DISK_DIR')}/data/Inquisition.gif"), mention_author=True)
						wordsSinceSpanishInquisition = 0
			else:
				#If file gets OBLITERATED again, repopulate it.
				wordsSinceSpanishInquisition = 0

		with open(spainFilePath, "w") as spainFile:
			spainFile.write(str(wordsSinceSpanishInquisition))

	else:
		#If file gets OBLITERATED again, remake it.
		with open(spainFilePath, "x") as spainFile:
			spainFile.write("0")



	if messageData.startswith("/updaterepo"): #Update textfiles repo (pull)
		await updateRepo(message=message)
		parseIndexFile();
		return

	elif messageData.startswith("/backupdata"): #Push data files to git repo
		backupData()
		await replyMessage(message, "Successfully pushed data to repo.", ping=True)
		return

	elif messageData.startswith("/pulldata"): #Pull data files from git repo
		pullBackupData()
		await replyMessage(message, "Successfully pulled data from repo.", ping=True)
		return



	elif messageData.startswith("/econ force-reload"): #Force-reload econ.
		importlib.reload(games.economy)
		return



	elif messageData.startswith("/uptime"): #Time DThree has been online for.
		currentTime = time.time()
		uptime = currentTime - D3StartTime


		days = int(uptime // 84600 % 365)
		hours = int(uptime // 3600 % 24)
		minutes = int(uptime // 60 % 60)
		seconds = int(uptime % 60)
		uptimeStr = ""
		if days > 0:
			uptimeStr += f"{days} days, "
		if hours > 0 or days > 0:
			uptimeStr += f"{hours} hours, "
		if minutes > 0 or hours > 0 or days > 0:
			uptimeStr += f"{minutes} minutes "
		uptimeStr += f"{seconds} seconds."


		timeSinceCreationStr = timeSinceStr("2024-09-01 08:00:00")

		await replyMessage(message, f"DThree has been online for: {uptimeStr}\nTime since DThree was created: {timeSinceCreationStr}", ping=True)
		return
	

	elif messageData.startswith("/whatis"): #Search with DDG.
		#Youtube results can embed; Others cannot.
		await replyMessage(message, "Processing query.", ping=True)
		query = messageData.replace("/whatis","").strip().lower()
		results = lookUp(query)
		if not results: #Empty or None.
			await replyMessage(message, "No meaningful results were found.", ping=True)
		else:
			reply = f"## Query: {query}\n"
			for title, url in results.items():
				if "youtube.com" not in url.lower():
					reply += f"- [{title}](<{url}>)\n"
				else:
					reply += f"- [{title}]({url}) *(YouTube link)*\n"
			reply += "-# *Some results may not be relevant. Results were screened and had SafeSearch enabled while being processed. Please be responsible with your searches.*"
			await replyMessage(message, reply, ping=True)
			return



	await checkReplies(messageData, message) #"/" Commands and such.
	await games.economy.econIterate(message, messageData) #/econ and related functions.
	await checkNoughtsAndCrossesGames(messageData, message) #Checks active ttt games.
	await games.BlackjackD3.checkJackBlackGames(messageData, message); #Checks active jb games.
	#await checkChessGames(userDisplayName, messageData, message) #Checks active chess games. Disabled as chess is still unfinished after 9 months (18/05/2025).
	#await testImage(message) #Debug chess function, probably unnecessary.
	await browseMemes(messageData, message) #Functions for "/browse".




async def handleReplyTask(message: discord.Message, repliedMessage: discord.Message) -> None:
	await replyToCorrection(message, repliedMessage)



def shouldNotSendMessage(message: discord.Message) -> bool:
	return not (os.path.exists(f"{os.getenv('TXT_DIR')}/d3.public") or message.guild.name == "Dau's Repository")


@client.event
async def on_message(message: discord.Message) -> None:
	if message.author == client.user:
		return #Don't self reply.
	if shouldNotSendMessage(message):
		#Stop replies in non-testing server if [not DTHREE_PUBLIC].
		return

	try:
		messageData = removeNonASCII(message.content.strip().lower())
		if message.reference and isinstance(message.reference.resolved, discord.Message):
			repliedMessage = message.reference.resolved
			if repliedMessage.author == client.user:
				await handleReplyTask(message, repliedMessage)
				return

		await otherTasks(message, messageData)
	
	except Exception as E:
		#Handle errors gracefully.
		print(f"\a\n{E}\n")
		await replyMessage(message, f"## *An error occurred;*\n{str(E)}\n-# *Please wait.*", ping=True)



async def main(token: str) -> None:
	bgTask = asyncio.create_task(backgroundActions(client))
	DThreeTask = asyncio.create_task(client.start(token))
	await asyncio.gather(bgTask, DThreeTask)


#Start everything
if __name__ == "__main__":
	token = os.getenv("BOT_TOKEN")
	asyncio.run(main(token))
