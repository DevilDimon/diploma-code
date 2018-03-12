import uuid

from mojom.parse.ast import Struct, Constraint, Interface, ComparisonPredicate


def GenerateDefinitions(tree, filename):
    res = '#pragma once\n'

    import_list = tree.import_list
    if import_list is not None:
        for import_item in import_list:
            res += "#include \"" + import_item.import_filename + ".h\"\n\n"

    res += "#include \"gene_embedded_types.h\"\n\n"
    res += "#include <cstdint>\n\n"

    if tree.module is not None:
        namespace = tree.module.mojom_namespace[1]
        res += 'namespace ' + namespace + ' {\n\n'

    for obj in tree.definition_list:
        if isinstance(obj, Struct):
            res += GenerateStruct(obj) + '\n'
        if isinstance(obj, Interface):
            res += GenerateInterface(obj) + '\n'
            res += GenerateInterfaceUserModeClient(obj) + '\n'

    if tree.module is not None:
        res += '\n}  // ' + namespace + '\n\n'

    res += GenerateSerializer(tree)

    file = open(filename + '.h', 'w')
    file.write(res)

    return res


def GenerateInterface(interface):
    res = 'class ' + interface.mojom_name + ' {\n'
    res += '\tprotected:\n'
    res += '\tconst uint64_t __service_id = ' + str(uuid.uuid1().int >> 64) + 'UL;\n'

    for method in interface.body.items:
        res += '\tconst uint64_t __' + method.mojom_name + '_id = ' + str(uuid.uuid1().int >> 64) + 'UL;\n'

    res += '\n'
    res += '\tpublic:\n'

    for method in interface.body.items:
        res += '\tvirtual bool ' + method.mojom_name + '('
        for arg in method.parameter_list:
            res += GenerateTypename(arg.typename) + ' ' + arg.mojom_name + ', '
        res = res[:-2] + ') = 0;\n'
    res += '\n};'

    return res

def GenerateInterfaceUserModeClient(interface):
    res = 'class ' + interface.mojom_name + 'Client final : public ' + interface.mojom_name + ' {\n'
    res += '\tpublic:\n'

    for method in interface.body.items:
        res += '\tbool ' + method.mojom_name + '('
        is_empty = True
        for arg in method.parameter_list:
            res += GenerateTypename(arg.typename) + ' ' + arg.mojom_name + ', '
            is_empty = False
        if not is_empty:
            res = res[:-2]
        res += ') final {\n'
        res += '\t\tgene_internal::container __c;\n'
        res += '\t\treturn gene_internal::serialize(__service_id, __c) &&\n'
        res += '\t\t\tgene_internal::serialize(__' + method.mojom_name + '_id, __c) &&\n'
        for arg in method.parameter_list:
            res += '\t\t\tgene_internal::serialize(' + arg.mojom_name + ', __c) &&\n'
        res += '\t\t\tgene_internal::send_message_internal(__c);\n'
        res += '\t}\n'
    res += '};\n'

    return res


def GenerateStruct(struct):
    res = 'struct ' + struct.mojom_name + ' {\n'

    for field in struct.body.items:
        typename = GenerateTypename(field.typename)
        array_brackets = typename.split('[')
        res += '\t' + GenerateTypename(typename.split('[', 1)[0]) + ' ' + field.mojom_name
        if len(array_brackets) > 1:
            for bracket in array_brackets[1:]:
                res += '[' + bracket


        if field.default_value is not None:
            res += ' = ' + field.default_value

        res += ';\n'
    return res + '\n};'

def GenerateTypename(typename):
    if typename == 'string':
        return 'std::string'
    elif typename == 'int8':
        return 'int8_t'
    elif typename == 'int16':
        return 'int16_t'
    elif typename == 'int32':
        return 'int32_t'
    elif typename == 'int64':
        return 'int64_t'
    elif typename.endswith('[]'):
        return GenerateArrayTypename(typename)
    else:
        return typename

def GenerateArrayTypename(typename):
    if typename.endswith('[]'):
        return 'std::vector<' + GenerateArrayTypename(typename[:-2]) + '> '
    else:
        return GenerateTypename(typename)


def GenerateSerializer(tree):
    constraints = {}
    for obj in tree.definition_list:
        if isinstance(obj, Constraint):
            constraints[obj.mojom_name] = obj.body.items

    res = 'namespace gene_internal {\n\n'
    if tree.module is not None:
        res += 'using namespace ' + tree.module.mojom_namespace[1] + ';\n\n'

    for obj in tree.definition_list:
        if isinstance(obj, Struct):
            res += GenerateStructSerializer(obj, constraints) + '\n};\n\n'
    res += '\n} // gene_internal\n'
    return res



def GenerateStructSerializer(struct, constraints):
    return GenerateSerializeOperator(struct, constraints) + GenerateDeserializeOperator(struct, constraints)

def GenerateSerializeOperator(struct, constraints):
    res = 'template <> struct serializer<' + struct.mojom_name + '> {\n'
    res += '\tbool operator()(const ' + struct.mojom_name + ' &v, container &c) {\n'

    # TODO: handle empty structs here and everywhere
    # if len(struct.body.items) == 0:
    #     res += 'true;\n}'
    #     return res

    field_number = 0
    constrained_fields = {}
    for field in struct.body.items:
        if field.attribute_list is not None and field.attribute_list.items[0].key in constraints:
            constraint = constraints[field.attribute_list.items[0].key]
            constrained_fields[field_number] = []
            constraint_number = 0
            for constraint_field in constraint:
                predicate = constraint_field.predicate
                var_name = 'c' + str(field_number) + '_' + str(constraint_number)
                if isinstance(predicate, ComparisonPredicate):
                    if predicate.mojom_name in ('size', 'value'):
                        name = predicate.mojom_name
                        if predicate.comp_op == '=':
                            res += '\t\t' + name + '_equals_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '!=':
                            res += '\t\t' + name + '_not_equals_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<':
                            res += '\t\t' + name + '_lesser_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '>':
                            res += '\t\t' + name + '_greater_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<=':
                            res += '\t\t' + 'compound_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_lesser_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '));\n'
                        elif predicate.comp_op == '>=':
                            res += '\t\t' + 'compound_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_greater_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '));\n'
                        constrained_fields[field_number].append(var_name)
                constraint_number += 1
        field_number += 1

    field_number = 0
    res += '\n\t\treturn\n'

    for field in struct.body.items:
        if field_number in constrained_fields:
            res += '\t\t\t'
            for var in constrained_fields[field_number]:
                res += var + '.check(v.' + field.mojom_name + ') && '
            res += 'serialize(v.' + field.mojom_name + ', c) &&\n'
        else:
            res += '\t\t\tserialize(v.' + field.mojom_name + ', c) &&\n'
        field_number += 1
    return res[:-4] + ';\n\t}\n'

def GenerateDeserializeOperator(struct, constraints):
    res = '\tbool operator()(container &c, ' + struct.mojom_name + ' *v) {\n'
    res += '\t\tif (!v)\n\t\t\treturn false;\n'

    field_number = 0
    constrained_fields = {}
    for field in struct.body.items:
        if field.attribute_list is not None and field.attribute_list.items[0].key in constraints:
            constraint = constraints[field.attribute_list.items[0].key]
            constrained_fields[field_number] = []
            constraint_number = 0
            for constraint_field in constraint:
                predicate = constraint_field.predicate
                var_name = 'c' + str(field_number) + '_' + str(constraint_number)
                if isinstance(predicate, ComparisonPredicate):
                    if predicate.mojom_name in ('size', 'value'):
                        name = predicate.mojom_name
                        if predicate.comp_op == '=':
                            res += '\t\t' + name + '_equals_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '!=':
                            res += '\t\t' + name + '_not_equals_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<':
                            res += '\t\t' + name + '_lesser_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '>':
                            res += '\t\t' + name + '_greater_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<=':
                            res += '\t\t' + 'compound_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_lesser_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '));\n'
                        elif predicate.comp_op == '>=':
                            res += '\t\t' + 'compound_constraint<' + GenerateTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_greater_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + GenerateTypename(field.typename) + '>(' + predicate.value + '));\n'
                        constrained_fields[field_number].append(var_name)
                constraint_number += 1
        field_number += 1

    field_number = 0
    res += '\n\t\treturn\n'

    for field in struct.body.items:
        if field_number in constrained_fields:
            res += '\t\t\tdeserialize(c, &v->' + field.mojom_name + ') && '
            for var in constrained_fields[field_number]:
                res += var + '.check(v->' + field.mojom_name + ') && '
            res += '\n'
        else:
            res += '\t\t\tdeserialize(c, &v->' + field.mojom_name + ') &&\n'
        field_number += 1
    return res[:-4] + ';\n\t}\n'