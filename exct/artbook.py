"artbook.py"
import xml.etree.ElementTree as ET;
import re as regex;
import random, os;



global BOOKS, TAGS;
ARTBOOK_DIR:str = f"{os.getenv('TXT_DIR')}/artbook";



class Page:
	def __init__(self, XML:ET.Element):
		attributes:dict[str,str] = XML.attrib;

		self.bookIndex:int = int(attributes["book"]); #The book this page is from.
		self.index:int = int(attributes["book"]);	  #Page number

		self.tags:list[str] = attributes["tags"].strip().lower().split(","); #Tags, what's on this page
		self.path:str = attributes["path"];									 #Filepath to image


		#Other setup;
		BOOKS[self.bookIndex].addPage(self);


	def __str__(self) -> str:
		return f"Page {self.index} from {BOOKS[self.bookIndex].name}, stored at filepath [{self.path}], tags: {self.tags}";

	def __repr__(self) -> str:
		return f"<Page [Index: {self.index}, Book: {self.bookIndex}, Path: [{self.path}], Tags: {self.tags}]>"

	def __contains__(self, value:str) -> bool:
		return value in self.tags;


class Book:
	def __init__(self, XML:ET.Element):
		attributes:dict[str,str] = XML.attrib;
		
		self.index:int = int(attributes["index"]);		#Book index

		self.name:str = attributes["name"];				#Book name
		self.mediaType:str = attributes["media"];		#What sort of media is it from?
		self.sourceMedia:str = attributes["source"];	#Name of the source IP
		
		self.ISBN13:str = attributes["ISBN13"];			#ISBN 13 number
		self.acronym:str = attributes["acronym"];		#Used in the file extensions for pages from this book.

		self.pages:list[Page] = []; #Will be programmatically filled later.
		self._tags:set[str] = set(); #Empty set to contain this book's tags.


	def __str__(self) -> str:
		return f"{self.name} - {len(self.pages)} pages, from '{self.sourceMedia}' [{self.mediaType.title()}], uses suffix '{self.acronym}'. ISBN13: {self.ISBN13}";

	def __repr__(self) -> str:
		return f"<Book [Index: {self.index}, Name: '{self.name}', Pages: {len(self.pages)}, Source: '{self.sourceMedia}' [{self.mediaType}], ISBN13: {self.ISBN13}]>"

	def __contains__(self, value:str) -> bool:
		return value in self._tags;

	def addPage(self, page:Page) -> None:
		""" Adds this page to the book, and adds its tag to the book's set for searching. """
		self.pages.append(page);
		for tag in page.tags: self._tags.add(tag);

	def find(self, tag:str) -> list[int]:
		""" Finds the page numbers of relevant pages in the book """
		return [
			Pnumber for (Pnumber, page) in enumerate(self.pages) if tag in page
		];

	def getPaths(self, indices:list[int]) -> list[str]:
		""" Returns the image paths for this list of indices """
		return [self.pages[i].path for i in indices];


BOOKS:list[Book] = []; #All books in the index file.



def parseIndexFile() -> None:
	tree:ET = ET.parse(f"{ARTBOOK_DIR}/index.artbook.xml");
	root:ET.Element = tree.getroot();

	booksNode:ET.Element = root[0];
	pagesNode:ET.Element = root[1];

	for xml in booksNode: BOOKS.append(Book(xml));
	for xml in pagesNode: Page(xml);



def filePathSortFunction(a) -> bool:
	try:
		return int(regex.sub(
			r"(?i)^.*/|\..*\.jpg$", "",
			a
		)); #Return just the page number.
	except ValueError: #Failed to convert to int; filepath may be invalid.
		return 0;


def searchArtbookIndexFile(tag:str) -> list[str]:
	relevantFilePaths:list[str] = [];
	for book in BOOKS:
		if (tag not in book): continue; #Skip this book, as it doesnt contain the tag.
		relevantPageNumbers:list[int] = book.find(tag);
		relevantFilePaths.extend(book.getPaths(relevantPageNumbers));


	finalPaths:list[str] = [f"{ARTBOOK_DIR}/{fp}" for fp in relevantFilePaths];
	if (len(finalPaths) > 10): #Over max file limit, select random 10.
		sample:list[str] = random.sample(finalPaths, 10);
		return sorted(sample, key=filePathSortFunction);
	else: return finalPaths; #Otherwise, return the entire list.



parseIndexFile();
if (__name__ == "__main__"):
	#Debug
	print(artbookSearch("/artbook concept"));