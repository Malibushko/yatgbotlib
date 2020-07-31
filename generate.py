import html
import time
from copy import copy
import re
import html2markdown
import requests
from bs4 import BeautifulSoup
import os
import json
import sys
import collections

os.makedirs("public", exist_ok=True)
TYPE_OVERRIDES = {
    "String": "str",
    "Integer": "int",
    "Boolean": "bool"
}
BOT_API_URL = "https://core.telegram.org/bots/api"
REPLACEMENTS = {
    "\u2019": "'",
    "\u2018": "'",
    "\u201c": '"',
    "\u201d": '"',
    "\u2014": "-",
    "\u2013": "-",
    "More info on Sending Files »": ""
}


class Version:
    def __init__(self, major, minor, patch=None):
        self.major = int(major)
        self.minor = int(minor)
        if patch:
            self.patch = int(patch)
        else:
            self.patch = None

    def __str__(self):
        if self.patch:
            return "{}.{}patch{}".format(self.major, self.minor, self.patch)
        else:
            return "{}.{}".format(self.major, self.minor)


def serialize(obj):
    """JSON serializer for objects not serializable by default json code"""

    if isinstance(obj, Version):
        return str(obj)

    return obj.__dict__


def escape_description(description):
    for repl in REPLACEMENTS.items():
        description = description.replace(*repl)
    return description


def determine_return(description_soup):
    return_info = description_soup.text
    if "array" in return_info.lower():
        is_array = True
    else:
        is_array = False
    return_type = []
    links = description_soup.find_all("a")
    links.reverse()
    for item in links:
        if item.text[0].isupper():
            for sentence in return_info.split("."):
                sentence = sentence.lower()
                if item.text.lower() in sentence and "return" in sentence:
                    return_type.append(item.text)
    if is_array:
        return ["array({})".format(return_type[0])]
    else:
        for item in description_soup.find_all("em"):
            if item.text[0].isupper():
                return_type.append(item.text)
        return return_type


def determine_argtype(argtype):
    if argtype.startswith("Array of"):
        argtype = "array({0})".format(determine_argtype(argtype.replace("Array of ", "", 1)))

    if argtype in TYPE_OVERRIDES:
        argtype = TYPE_OVERRIDES[argtype]
    return argtype


def determine_arguments(description_soup):
    if "requires no parameters" in description_soup.text.lower():
        return {}
    else:
        table = description_soup.find_next_sibling("table")
        arguments = {}
        for row in table.find_all("tr")[1:]:  # Skip first row (headers)
            row = row.find_all("td")
            description_soup = row[-1]
            argdata = {"types": [], "description": gen_description(description_soup)}
            if len(row) == 4:
                argdata["required"] = True if row[2].text == "Yes" else False
            else:
                argdata["required"] = not row[-1].text.startswith("Optional.")
            argtypes = row[1].text.split(" or ")
            for argtype in argtypes:
                argtype = argtype.strip()
                argtype = determine_argtype(argtype)
                argdata["types"].append(argtype)
            arguments[row[0].text] = argdata
        return arguments


def gen_build_info():
    build_info = {}
    if os.getenv("CI", False):
        print("Building on CI")
        build_info["branch"] = os.getenv("CI_COMMIT_REF_NAME")
        build_info["commit"] = "%s (%s), build #%s, reason: %s" % (
            os.getenv("CI_COMMIT_SHORT_SHA"), os.getenv("CI_COMMIT_MESSAGE").replace("\n", ''),
            os.getenv("CI_PIPELINE_IID"),
            os.getenv("CI_PIPELINE_SOURCE"))
        build_info["pipeline_url"] = os.getenv("CI_PIPELINE_URL")
    else:
        print("Building locally.")
        build_info["branch"] = None
        build_info["commit"] = None
        build_info["pipeline_url"] = None
    build_info["timestamp"] = int(time.time())
    return build_info


def get_html(soup):
    for link in soup.find_all("a"):
        if link["href"].startswith("#") and not link.text == "":
            if "-" in link["href"]:
                # Article
                link["href"] = "#/articles/%s" % link["href"].split("#")[1]
            elif link.text[0].islower():
                # Method
                link["href"] = "#/methods/%s" % link.text
            else:
                # Probably type
                link["href"] = "#/types/%s" % link.text
    return str(soup).replace("<td>", "").replace("</td>", "").replace("<body>", "").replace("</body>", "")


def gen_description(soup):
    description = {}
    description["plaintext"] = escape_description(soup.text)
    description["html"] = get_html(soup)
    description["markdown"] = html.unescape(html2markdown.convert(description["html"]))
    return description


def get_article(description_soup):
    articles_text = str(description_soup)
    for sibling in list(description_soup.next_siblings):
        if sibling.name in ["h3", "h4"]:
            break
        else:
            articles_text += str(sibling)
    articles_soup = BeautifulSoup(articles_text, "lxml")
    article = gen_description(articles_soup.find("body"))
    return article


def generate_bot_api_data(schema, dwn_url=BOT_API_URL, update_version=False, changelog=False):
    r = requests.get(dwn_url)
    soup = BeautifulSoup(r.text, features="lxml")
    if update_version:
        schema["version"] = soup.find_all("strong")[2].text.lstrip("Bot API ")
    sections = soup.find_all(["h3", "h4"])
    if changelog:
        sections.reverse()
    for section in sections:
        title = section.text
        description_soup = section.find_next_sibling()
        if not description_soup:
            continue
        category = description_soup.find_previous_sibling("h3").text
        if changelog:
            article = get_article(description_soup)
            version = description_soup.find("strong")
            if version and version.text.startswith("Bot API "):
                article["version"] = Version(*version.text.lstrip("Bot API ").split("."))
            elif len(schema["changelogs"]) > 0:
                previous = list(schema["changelogs"].values())[-1]["version"]
                article["version"] = copy(previous)
                if article["version"].patch:
                    article["version"].patch += 1
                else:
                    article["version"].patch = 1
            else:
                article["version"] = Version(1, 0)
            schema["changelogs"][title] = article
            print("Adding changelog", title)
        elif " " in title:
            if "Recent changes" in category:
                print("Changelog article, skipping to be added later")
                continue
            article_id = section.find("a")["name"]
            article = get_article(description_soup)
            article["title"] = title
            article["category"] = category
            schema["articles"][article_id] = article
            print("Adding article", title, "of category", category)
        elif title[0].islower():
            method = {"arguments": determine_arguments(description_soup),
                      "returns": determine_return(description_soup),
                      "description": gen_description(description_soup),
                      "category": category}
            schema["methods"][title] = method
            print("Adding method", title, "of category", category)
        else:
            type_ = {"fields": determine_arguments(description_soup),
                     "description": gen_description(description_soup),
                     "category": category}
            schema["types"][title] = type_
            print("Adding type", title, "of category", category)


def generate_schema():
    schema = {"types": {}, "methods": {}, "articles": {}, "changelogs": {}, "version": "Not found yet",
              "build_info": gen_build_info()}
    generate_bot_api_data(schema, update_version=True)
    print("Built schema for Bot API version", schema["version"])
    print("Getting changelogs...")
    generate_bot_api_data(schema, "https://core.telegram.org/bots/api-changelog", changelog=True)
    print(len(schema["types"]), "types")
    print(len(schema["methods"]), "methods")
    print("Build info:", schema["build_info"])
    with open("public/all.json", 'w') as f:
        json.dump(schema, f, indent=4, default=serialize)
    for name, data in schema.items():
        if name == "version":
            extension = "txt"
            write_data = data
        else:
            extension = "json"
            write_data = json.dumps(data, indent=4, default=serialize)
        with open("public/{}.{}".format(name, extension), 'w') as f:
            f.write(write_data)


def map_tgvalue_to_cpp(value, required=False, is_pointer=False):
    mapper = {'str': 'std::string',
              'String':'std::string',
              'int': 'int64_t',
              'True': 'bool',
              'False': 'bool',
              'true': 'bool',
              'false': 'bool',
              'Int':'int64_t',
              'Float': 'float',
              'Float number': 'float',
              'Messages':'Message'}

    array_regex = re.compile("array\((.+)\)")
    is_array = array_regex.search(value)
    if is_array:
        value = "std::vector<{}>".format(map_tgvalue_to_cpp(is_array.group(1), True))

    ptr_regex = re.compile("ptr\((.+)\)")
    is_ptr = ptr_regex.search(value)
    if is_ptr:
        value = "std::unique_ptr<{}>".format(is_ptr.group(1))

    if value in mapper:
        value = mapper[value]

    if is_pointer:
        value = "std::unique_ptr<{}>".format(value)

    if not required:
        value = "std::optional<{}>".format(value)
    return value


def print_struct(fields, key, value):
    print("/// {}\nstruct {} {{\ndeclare_struct".format(value['description']['plaintext'], key))
    for field in fields:
        cur_field = fields[field]
        field_type = cur_field['types'][0]
        field_required = cur_field['required']
        field_is_pointer = (str(field_type).strip().casefold() == str(key).strip().casefold())
        field_description = cur_field['description']['plaintext']
        print("declare_field({},{}); /// {}".format(map_tgvalue_to_cpp(field_type, field_required, field_is_pointer),
                                                    field, field_description))
    print("};")


def all_fields_processed(key, fields, processed_keys, tg_types):
    for field in fields:
        field_type = fields[field]['types'][0]
        if field_type in tg_types \
                and field_type not in processed_keys \
                and str(key).casefold().strip() != str(field_type).casefold().strip():
         #   print("Field {} cannot be proccessed".format(field_type))
            return False

    return True


def all_is(keys,value):
    for key in keys:
        if keys[key] != value:
            return False
    return True


def json_equal(lhs,rhs):
    return str(lhs).casefold().strip() == str(rhs).casefold().strip()

def generate_types(input,output):
    original_stdout = sys.stdout
    input_file = open(input, 'r')
    output_file = open(output,'w')
    sys.stdout = output_file
    print(input_file.read())

    types = open("public/types.json", "r")
    exclude_keys = {"InputFile": "using InputFile = File;",
                    "InputMessageContent": "using InputMessageContent = "
                                           "std::variant<InputTextMessageContent,"
                                           "InputLocationMessageContent,"
                                           "InputVenueMessageContent,"
                                           "InputContactMessageContent>;"}
    suspend_keys = {}
    processed_keys = {}
    tg_types = {}
    input_types = {"InputTextMessageContent":0,"InputLocationMessageContent":0,
                   "InputVenueMessageContent":0,"InputContactMessageContent":0}
    input_printed = False

    parsed_json = json.loads(types.read())
    for key in parsed_json:
        if key in exclude_keys:
            continue
        else:
            tg_types[key] =key
            print("struct {};".format(key))

    tg_types["InputMessageContent"] = "InputMessageContent"

    for key in exclude_keys:
        print(exclude_keys[key])

    for key in parsed_json:
        if key in exclude_keys:
            continue
        fields = parsed_json[key]["fields"]

        key_suspended = False
        for field in fields:
            field_type = fields[field]['types'][0]
            if field_type in tg_types and field_type not in processed_keys:
                suspend_keys.__setitem__(key, key)
                key_suspended = True
                break

        if key_suspended:
            continue

        print_struct(fields, key, parsed_json[key])
        processed_keys.__setitem__(key, key)

        if not input_printed:
            for input_key in input_types:
                if json_equal(input_key,key):
                    input_types[input_key] = 1

            if all_is(input_types,1):
                print(exclude_keys["InputMessageContent"])
                processed_keys.__setitem__("InputMessageContent","InputMessageContent")
                input_printed = True

    print_count = 0
    while print_count != len(suspend_keys):
        for key in suspend_keys:
            if key not in processed_keys:
           #     print("Trying to process {}".format(key))
                if all_fields_processed(key, parsed_json[key]["fields"], processed_keys, tg_types):
                    print_struct(parsed_json[key]["fields"], key, parsed_json[key])
                    processed_keys.__setitem__(key, key)
                    print_count += 1
                elif key == "Chat" or key == "CallbackQuery":
                    chat_fields = parsed_json[key]["fields"]
                    for chat_field in chat_fields:
                        chat_field_type = chat_fields[chat_field]['types'][0]
                        if chat_field_type not in processed_keys and chat_field_type in tg_types:
                            chat_fields[chat_field]['types'][0] = "ptr({})".format(chat_field_type)

                    print_struct(chat_fields,key,parsed_json[key])
                    processed_keys.__setitem__(key, key)
                    print_count += 1

    print("}")
    sys.stdout = original_stdout
    output_file.closed
    input_file.closed


def generate_methods(input,output,declarations_only=False):
    original_stdout = sys.stdout
    input_file = open(input, 'r')
    output_file = open(output,'w')
    sys.stdout = output_file
    print(input_file.read())

    methods = open("public/methods.json", "r")
    parsed_json = json.loads(methods.read())

    for key in parsed_json:
        if key == 'sendMediaGroup':
            continue
        return_type = ""
        return_type_arr = parsed_json[key]['returns']
        unique_types = []
        if len(return_type) == 0:
            return_type = "bool"
        if len(return_type_arr) == 1:
            return_type = return_type_arr[0]
        if len(return_type_arr) > 1:
            for type in return_type_arr:
                unique_types.append(map_tgvalue_to_cpp(type,True))

            unique_types = list(dict.fromkeys(unique_types))
            unique_types.sort(key=lambda x: x[0].isupper())

            return_type = "std::variant<"+ ', '.join(str(e) for e in unique_types)
            return_type += '>'

        return_type = map_tgvalue_to_cpp(return_type,True)
        if not declarations_only:
            print("std::pair<{1},opt_error> Bot::{0} ( ".format(key,return_type))
        else:
            print("/**\n\t@brief {}".format(parsed_json[key]["description"]["plaintext"]))
            for args in parsed_json[key]["arguments"]:
                print("\t@param {} {}".format(args,parsed_json[key]["arguments"][args]["description"]["plaintext"]))
            print("\t@returns {}".format(return_type))
            print ("*/")
            print("std::pair<{1},opt_error> {0} ( ".format(key, return_type))

        arguments = parsed_json[key]['arguments']

        input_files = []
        item = []
        elems = 0
        for arg in arguments:
            arg_type = ""
            arg_types = arguments[arg]['types']
            if len(arg_types) > 1:
                arg_type = "std::variant<"
                counter = len(arg_types)
                for type_elem in arg_types:
                    counter -= 1
                    arg_type += map_tgvalue_to_cpp(type_elem,True)
                    arg_type += ',' if counter else '>'
            else:
                arg_type = arg_types[0]

            if "InputFile" in arg_types:
                arg_type = "std::string"
                input_files.append(arg)

            required = arguments[arg]['required']
            elems += 1
            arg_type = map_tgvalue_to_cpp(arg_type,required)
            if "std::string" in arg_type:
                arg_type = "const {}& ".format(arg_type)
            else:
                for letter in arg_type:
                    if letter.isupper():
                        arg_type = "const {}& ".format(arg_type)
                        break
            item.append('\t'*15+ '{}\t{}{}'.format(arg_type,arg,
                                                   '={}' if declarations_only and not required else ''))
        item.sort(key=lambda x: 'std::optional' in x)
        print(',\n'.join(item))
        if not declarations_only:
            print(') const {')
        else:
            print(') const;')
            continue

        if len(arguments) == 0:
            print("return api->ApiCall<{}>(__func__);\n}}".format(return_type))
            continue
        print("QueryBuilder builder;\nbuilder",end='')

        for arg in arguments:
            elems -= 1
            if arg in input_files:
                continue
            separator = '' if elems else ';'
            print(" << make_named_pair({}){}".format(arg,separator))
        print(';')

        if len(input_files) > 0:
            print("std::vector<name_value_pair> params;\nparams.reserve({});".format(len(input_files)))
            for arg in input_files:
                if arguments[arg]['required']:
                    print("params.push_back(name_value_pair{{\"{}\",std::move({})}});".format(arg,arg))
                else:
                    print("if ({}.has_value())\n\tparams.push_back(name_value_pair{{\"{}\",std::move({}.value())}});".format(
                        arg,arg,arg))

        if len(input_files) == 0 and len(unique_types) == 0:
            print("return api->ApiCall<{}>(__func__,builder);".format(return_type))
        elif len(input_files) > 0:
            print("return api->ApiCall<{}>(__func__,builder,params);".format(return_type))
        elif len(unique_types) > 0:
            print("return api->ApiCall<{},{}>(__func__,builder);".format(return_type,unique_types.pop()))

        print('}')
    if declarations_only:
        print("};\n}")
    else:
        print("}")
    sys.stdout = original_stdout
    output_file.closed
    input_file.closed


if __name__ == '__main__':
    generate_schema()
    generate_types(sys.argv[1],sys.argv[2])
    generate_methods(sys.argv[3],sys.argv[4],True)
    generate_methods(sys.argv[5],sys.argv[6],False)