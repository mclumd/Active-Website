#include <stdio.h>
#include <ctype.h>
#define NUM_FIELDS 26
#define NUM_TYPES 14
#define SSIZE1 40
#define LLENGTH 100

typedef enum {
  address,
  annote,
  author,
  booktitle,
  chapter,
  crossref,
  edition,
  editor,
  howpublished,
  institution,
  journal,
  key,
  month,
  note,
  number,
  organisation,
  pages,
  publisher,
  school,
  series,
  title,
  type,
  volume,
  year,
  keywords,
  url
} Bibfields;


typedef enum {
  article,
  book,
  booklet,
  conference,
  inbook,
  incollection,
  inproceedings,
  manual,
  mastersthesis,
  misc,
  phdthesis,
  proceedings,
  techreport,
  unpublished
} Bibtypes;

char Fields[][20] = {
  "address",
  "annote",
  "author",
  "booktitle",
  "chapter",
  "crossref",
  "edition",
  "editor",
  "howpublished",
  "institution",
  "journal",
  "key",
  "month",
  "note",
  "number",
  "organisation",
  "pages",
  "publisher",
  "school",
  "series",
  "title",
  "type",
  "volume",
  "year",
  "keywords",
  "url"
};

char Types[][20] = {
  "article",
  "book",
  "booklet",
  "conference",
  "inbook",
  "incollection",
  "inproceedings",
  "manual",
  "mastersthesis",
  "misc",
  "phdthesis",
  "proceedings",
  "techreport",
  "unpublished"
};

struct kwd_foo{
  struct kwd_foo next;
  char *kwd;
};

typedef struct kwd_foo kwdlist;


struct auth_foo{
  struct auth_foo next;
  char *auth;
};

typedef struct auth_foo authlist;

typedef struct {
  Bibtypes type;
  char (*fields)[NUM_FIELDS];
  kwdlist* keywords;
  authlist* authors;
} entry;

struct el_foo {
  struct el_foo *next;
  entry entry;
};

typedef struct el_foo elist;


entry* get_entry(FILE*);
int get_type(entry*, char*);
int get_field(entry*, char*);
int f2i(char*);			/* convert field name to int */
int get_authors(entry*);
int get_keywords(entry*);


int get_authors(entry* entry1){
  int cnt1 = 0, ncnt = 0;
  char name1[SSIZE1], *line;

  line = entry1->fields[author];
  while(isspace(line[cnt1])) cnt1++;
  bzero(name1, SSIZE1);
  name1[ncnt] = line[cnt1];
				/* if there is a 0, end the function */
				/* if there is a ',' end this author */
				/* if there is a " and" end this author */
  while(line[cnt1] != ',');
  


}


int get_type(entry* entry1, char* type1){
  int cnt1 = 0;
  while(strcasecmp(type1, Types[cnt1]) != 0 && cnt1 < NUM_TYPES) cnt1++;
  if(cnt1 >= NUM_TYPES) return -1;
  entry1->type = cnt1;
  return cnt1;
}

int f2i(char* name){
  int cnt1 = 0;
  while(strcasecmp(name, Fields[cnt1]) != 0 && cnt1 < NUM_FIELDS) cnt1++;
  if(cnt1 >= NUM_FIELDS) return -1;
  return cnt1;
}


int get_field(entry* entry1, char* line1){
  char char1, fname1[SSIZE1], fval[LLENGTH];
  int cnt1 = 0, fld = -1, incnt = 0, fcnt= 0;

  bzero(fname1, SSIZE1);
  bzero(fval, LLENGTH);
				/* skip whitespace */
  while(isspace(line1[incnt])) incnt++;
				/* get field name */
  fname1[cnt1] = line1[incnt];
  cnt1++;
  incnt++;
  while(!isspace(line1[incnt]) && line1[incnt] != '='){
    fname1[cnt1] = line1[incnt];
    cnt1++; incnt++;
  }
  fld = f2i(fname1);
  if(fld == -1) return -1;	/* error in field name */
				/* get field value */
  while(isspace(line1[incnt])) incnt++;
  if(char1 == '='){
    incnt++;
    while(isspace(line1[incnt])) incnt++;
  }
  if(line1[incnt] == '\"'){
    incnt++;
    while(line1[incnt] != '\"' ){
      fval[fcnt] = line1[incnt];
      incnt++; fcnt++;
    }
  }
  else{
    while(line1[incnt] != ',' && line1[incnt] != '\n' ){
      fval[fcnt] = line1[incnt];
      incnt++; fcnt++;
    }
  }
  entry1->fields[fld] = (char*) malloc(strlen(fval) + 1);
  strncpy(entry1->fields[fld], fval, strlen(fval) + 1);
  
  if(fld == author)
    if(get_authors(entry1) == -1) return -1;
  if(fld == keywords)
    if(get_keywords(entry1) == -1) return -1;

  return 0;
}




entry* get_entry(FILE* infile){
  char char1, string1[SSIZE1], line[LLENGTH];
  int cnt1;
  entry *entry1;

  entry1 = (entry*) malloc(sizeof(entry));
  entry1->keywords = NULL;
  entry1->authors = NULL;
				/* eat white spaces */
  char1 = fgetc(infile);
  while(isspace(char1) && ! feof(infile)) char1 = fgetc(infile);

  if(feof(infile)) return NULL:
  if(char1 != '@') return NULL;
				/* get entry type */
  bzero(string1, SSIZE1); cnt1 = 0;
  char1 = fgetc(infile);
  while(char1 != '{'){
    string1[cnt1] = char1;
    char1 = fgetc(infile);
    cnt1++;
  }
  if(get_type(entry1, string1) == -1) return NULL;
				/* skip to end of line */
  while(char1 != '\n') char1 = fgetc(infile);
				/* get the fields */
  bzero(line, LLENGTH);
  fgets(line, LLENGTH, infile);
  while(get_field(entry1, line) != -1){
      bzero(line, LLENGTH);
      fgets(line, LLENGTH, infile);
    }
  return entry1;

}




main(){}
