import random;
import discord;
#from exct.shared import sendMessage, replyMessage;


class JBGame:
	def __init__(self, player:discord.Member) -> None:
		self._player:discord.Member = player;
		self.playerCard:int 		= random.randint(1, 10);
		self.d3Card:int 			= random.randint(1, 10);

activeGames:dict[str, JBGame] = {};


#Shouldn't print to console unconditionally, but I don't want to remove this.
'reply += "Welcome to Shabbles\' Blackjack!")'


def forceEndAllJB() -> None:
	"""
	Forcefully ends all ongoing JB games.
	"""
	IDs = list(activeGames.keys());
	del activeGames;
	print(f"Ended {len(IDs)} games.");





async def handleChallenge(messageData:str, message: discord.Message) -> JBGame|None:
	#Handle challenge command [Calling for start of a game]
	if messageData.startswith("/challenge") and ((a in messageData) for a in ("jb", "bj", "blackjack", "jackblack")):
		
		if ((message.author) in activeGames.keys()):
			await replyMessage(message, f"{message.author.display_name}, you cannot start a new game while yours is active. Use `/play r` to reset.", ping=True);
			return activeGames[message.author.name];
		else:
			activeGames.append(JBGame(message.author));
			await replyMessage(message, f"{message.author.display_name}, you have challenged DThree to a game of BlackJack!", ping=True);
			return activeGames[-1];

	else:
		if ((message.author) in activeGames.keys()):
			return activeGames[message.author.name];
		else:
			await replyMessage(message, f"{message.author.display_name}, you need to start a game first, with `/challenge bj`.", ping=True);
			return None;



async def checkJackBlackGames(messageData: str, message: discord.Message) -> None:
	"""
	Checks if response is related to an ongoing game.
	If so, checks validity.
	If so, plays move, then checks for a winner.
	If so, then tells players about winner and ends the ongoing game.
	"""

	game:JBGame|None = handleChallenge(messageData, message);
	if (not JBGame):
		return;

	if (not messageData.startswith("/play")): return;
	splitMessage:list[str] = messageData.split(" ");
	if (len(splitMessage) < 2): return;
	userChoice = splitMessage[1];

	reply:str = "";

	match(userChoice):
		case "y":
			game.playerCard += random.randint(1, 10);
			if (game.playerCard > 21):
				reply += "Bot win!\n";

			elif (game.playerCard == 21):
				reply += "User win!\n";

			reply += f"User: {game.playerCard}\n";
			reply += f"Bot: {game.d3Card}\n";

			if ((game.d3Card < 15) or (random.randint(1,3) == 3)):
				game.d3Card += random.randint(1, 10);
				if ((game.d3Card > 21) or (game.playerCard == 21)):
					reply += "User win!\n";
				reply += f"User: {game.playerCard}\nBot: {game.d3Card}\n";

			reply += f"Bot: {game.d3Card}\n";



		case "n":
			while ((game.d3Card < 15) or (random.randint(1, 3) == 3)):
				game.d3Card += random.randint(1, 10);
				if ((game.d3Card > 21) or (game.playerCard == 21)):
					reply += "User win!\n";
				reply += f"User: {game.playerCard}\n";
				reply += f"Bot: {game.d3Card}\n";

			
			reply += f"User: {game.playerCard}\n";
			reply += f"Bot: {game.d3Card}\n";



		case "f": #Fold
			game.d3Card += random.randint(1, 10);
			if ((game.d3Card > game.playerCard) and (game.d3Card <= 21)):
				reply += "Bot win!\n";

			elif ((game.playerCard > game.d3Card) and (game.playerCard <= 21)):
				reply += "User win!\n";
			
			reply += f"User: {game.playerCard}\n";
			reply += f"Bot: {game.d3Card}\n";



		case "r": #Reset
			game.playerCard = 0;
			game.d3Card = 0;


		case _: #Default
			return; #No action required.

	if (not reply.empty()):
		await replyMessage(message, reply, ping=True);


	activeGames[game.player] = game;
